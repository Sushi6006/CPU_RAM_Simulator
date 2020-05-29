#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

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
    status_list = (status_list_t*)malloc(sizeof(*status_list));
    if (status_list == NULL) {
        perror("ERROR creating status list");
        exit(EXIT_FAILURE);
    }
    // initialise as an empty list with all memory available
    status_list->head->status = -1;
    status_list->head->start = 0;
    status_list->head->next = status_list->foot = NULL;

    return status_list;
}

// update status list with given memory
status_list_t *update_status(status_list_t *status_list, unit_t *memory_list) {
    // TODO: finish
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

// allocate process into memory, return new status list
status_list_t *allocate_proc(unit_t *memory_list, status_list_t *status_list, process_t *proc, int time) {

    if ((memory_list == NULL) || (status_list == NULL) || (proc == NULL)) {
        perror("ERROR allocating process to memory");
        exit(EXIT_FAILURE);
    }

    int position = has_enough(status_list, proc->mem_req);
    if (position >= 0) {  // has space
        for (int i = 0; i < proc->mem_req; i++) {
            memory_list[position].proc_id = proc->id;
            memory_list[position].time_used = time;
            position++;
        }
        // TODO: update status
    } else {
        // not enough space
        // evict process until has enough space
        // TODO: duh
    }

}