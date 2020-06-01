#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "scheduler.h"
#include "memory.h"
#include "processlist.h"
#include "output.h"

// create an empty list where all memory available
unit_t *init_memory_list(int size) {
    unit_t *memory_list = (unit_t*)malloc(size * sizeof(unit_t));
    for (int i = 0; i < size; i++) {
        memory_list[i].proc_id = HOLE;    // not occupied
        memory_list[i].time_used = HOLE;  // never used
    }
    return memory_list;
}

// create a list of memory status with all memory available
status_list_t *init_status_list(int size) {
    status_list_t *status_list;
    status_list = (status_list_t*)malloc(size * sizeof(*status_list));
    if (status_list == NULL) {
        perror("ERROR creating status list");
        exit(EXIT_FAILURE);
    }
    // initialise as an empty list with all memory available
    status_list->head = status_list->foot = NULL;
    status_list = add_status(status_list, create_status(HOLE, 0, size));

    return status_list;
}

// check if process is in memory
int mem_has_proc(unit_t *mem, int size, int proc_id) {
    for (int i = 0; i < size; i++) {
        if (mem[i].proc_id == proc_id) {
            return 1;
        }
    }
    return 0;
}

int mem_occupied(unit_t *mem, int size, int *proc_mem, int proc_id) {
    int count = 0;
    int proc_count = 0;
    for (int i = 0; i < size; i++) {
        if (mem[i].proc_id != HOLE) {
            count++;
        }
        if (mem[i].proc_id == proc_id) {
            proc_mem[proc_count++] = i;
        }
    }
    return count;
}

// check if theres a block of memory fit the need
// return: -1 if no, the address if available
int has_enough(status_list_t *status_list, int req_mem) {

    if (status_list == NULL) {
        perror("ERROR finding empty memory in null list");
        exit(EXIT_FAILURE);
    }
    status_t *status = status_list->head;
    while (status != NULL) {
        if ((status->status == HOLE) && (status->size >= req_mem)) {
            return status->start;
        }
        status = status->next;
    }
    
    // not found
    return -1;
}

// create a status node
status_t *create_status(int status, int start, int size) {
    status_t *new_status;
    new_status = (status_t*)malloc(sizeof(*new_status));
    if (new_status == NULL) {
        perror("ERROR creating new status node");
        exit(EXIT_FAILURE);
    }

    new_status->status = status;
    new_status->start = start;
    new_status->size = size;
    new_status->next = NULL;

    return new_status;
}

// add new status node
status_list_t *add_status(status_list_t *list, status_t *new_status) {
    if ((list == NULL) || (new_status == NULL)) {
        perror("ERROR adding status to list");
        exit(EXIT_FAILURE);
    }

    if ((list->head == NULL) || (list->foot == NULL)) {
        // first status node
        list->head = list->foot = new_status;
    } else {
        list->foot->next = new_status;
        list->foot = new_status;
    }

    return list;
}

// update status list with given memory
status_list_t *update_status(status_list_t *status_list, unit_t *memory_list, int len) {
    // init
    int last_status = memory_list[0].proc_id == HOLE ? HOLE : OCCU;
    int last_start = 0;
    // status_list_t *status_list;
    // i don't quite understand why its 64
    // but if i dont do this, i get 
    // `malloc(): unsorted double linked list corrupted`
    // and i have no clue why
    // status_list = (status_list_t*)malloc(64 * sizeof(*status_list));
    for (int i = 1; i < len; i++) {
        int curr_status = memory_list[i].proc_id == HOLE ? HOLE : OCCU;
        if (curr_status != last_status) {
            status_list = add_status(status_list,
                                     create_status(last_status, last_start, i - last_start));
            last_status = curr_status;
            last_start = i;
        }
    }
    status_list = add_status(status_list, create_status(last_status, last_start, len - last_start));
    return status_list;
}

// allocate process into memory, return new status list
status_list_t *swap_mem(unit_t *memory_list, int memsize, process_t *proc, int time) {

    if ((memory_list == NULL) || (proc == NULL)) {
        perror("ERROR allocating process to memory");
        exit(EXIT_FAILURE);
    }

    status_list_t *status_list = init_status_list(memsize);
    update_status(status_list, memory_list, memsize);
    int position;           // where to be allocated
    int evicted_count = 0;  // whether processes have been evicted
    int *evicted_add = (int*)malloc(memsize * sizeof(int));  // evicted addresses
    
    // evict processes until has enough space
    while ((position = has_enough(status_list, proc->mem_req)) == -1) {
        // find earliest proc to remove
        // find the first value and use it as the init value for min
        int min_time = -1, min_proc = -1;
        for (int i = 0; i < memsize; i++) {
            // this memory is occupied
            if (memory_list[i].proc_id != -1) {
                if ((min_time == -1)  || (memory_list[i].time_used < min_time)) {
                    min_time = memory_list[i].time_used;
                    min_proc = memory_list[i].proc_id;
                }
            }
        }
        
        // now proc_id is found, remove the process from memory
        evict_proc(memory_list, memsize, min_proc, evicted_add, &evicted_count);
        status_list = update_status(status_list, memory_list, memsize);
    }

    // print evict message
    if (evicted_count > 0) {
        // convert evicted add to string
        char *msg = (char*)malloc(MAX_MSG_LEN * sizeof(char));
        strcpy(msg, EVICTED_MSG);
        for (int i = 0; i < evicted_count; i++) {
            char addr_str[ADDR_STR_LEN];
            sprintf(addr_str, i < evicted_count - 1 ? "%d," : "%d]", evicted_add[i]);
            strcat(msg, addr_str);
        }

        // print messsage
        print_status(time, EVICTED, -1, msg);  // no proc id needed

        free(msg);
    }

    for (int i = position; i < position + proc->mem_req; i++) {
        memory_list[i].proc_id = proc->id;
        memory_list[i].time_used = time;
    }

    status_list = update_status(status_list, memory_list, memsize);

    return status_list;

}

// evict one process from memory
void evict_proc(unit_t *memory_list, int memsize, int proc_id, int *evicted_add, int *evicted_count) {
    
    for (int i = 0; i < memsize; i++) {
        if (memory_list[i].proc_id == proc_id) {
            memory_list[i].proc_id = HOLE;
            evicted_add[*evicted_count] = i;
            (*evicted_count)++;
        }
    }

}

// void *finish_proc(unit_t *memory_list, int memsize, int proc_id);
