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
unit_t *init_memory_list(long long size) {
    unit_t *memory_list = (unit_t*)malloc(size * sizeof(unit_t));
    for (long long i = 0; i < size; i++) {
        memory_list[i].proc_id = HOLE;    // not occupied
        memory_list[i].time_used = HOLE;  // never used
    }
    return memory_list;
}

// create a list of memory status with all memory available
status_list_t *init_status_list(long long size) {
    status_list_t *status_list;
    status_list = (status_list_t*)malloc(size * sizeof(*status_list));
    if (status_list == NULL) {
        perror("ERROR creating status list");
        exit(EXIT_FAILURE);
    }
    // initialise as an empty list with all memory available
    status_list->head = status_list->foot = NULL;

    return status_list;
}

// check if process is in memory for swapping
int mem_has_proc(unit_t *mem, long long size, long long proc_id) {
    for (long long i = 0; i < size; i++) {
        if (mem[i].proc_id == proc_id) {
            return 1;
        }
    }
    return 0;
}

// calculate memory usage, and also marked as used
long long calc_mem_usage(unit_t *mem, long long size) {
    long long count = 0;
    for (long long i = 0; i < size; i++) {
        if (mem[i].proc_id != HOLE) {
            count++;
        }
    }
    return count;
}

// calculate page count occupied by proc, and writes memory address to proc_mem
// and mark as used
long long calc_proc_usage(unit_t *mem, long long size, long long *proc_mem, long long proc_id, long long time) {
    long long proc_count = 0;
    for (long long i = 0; i < size; i++) {
        if (mem[i].proc_id == proc_id) {
            mem[i].time_used = time;
            proc_mem[proc_count++] = i;
        }
    }
    return proc_count;
}

// check if theres a block of memory fit the need
// return: -1 if no, the address if available
long long has_enough(status_list_t *status_list, long long req_mem) {

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

// sort the result of mem evicted
void sort_evicted_add(long long *evicted_add, long long count) {
    for (long long i = 0; i < count - 1; i++) {
        for (long long j = i + 1; j < count; j++) {
            if (evicted_add[j] <= evicted_add[i]) {
                long long temp = evicted_add[i];
                evicted_add[i] = evicted_add[j];
                evicted_add[j] = temp;
            }
        }
    }
}

// create a status node
status_t *create_status(int status, long long start, long long size) {
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
status_list_t *update_status(status_list_t *status_list, unit_t *memory_list, long long len) {
    // init
    int last_status = memory_list[0].proc_id == HOLE ? HOLE : OCCU;
    long long last_start = 0;
    for (long long i = 1; i < len; i++) {
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

// evict one process from memory
void evict_proc(unit_t *memory_list, long long memsize, long long proc_id, long long *evicted_add, long long *evicted_count) {
    
    for (long long i = 0; i < memsize; i++) {
        if (memory_list[i].proc_id == proc_id) {
            memory_list[i].proc_id = HOLE;
            evicted_add[*evicted_count] = i;
            (*evicted_count)++;
        }
    }

}

// allocate process into memory, return new status list
void swap_mem(unit_t *memory_list, long long memsize, process_t *proc, long long time) {

    if ((memory_list == NULL) || (proc == NULL)) {
        perror("ERROR allocating process to memory");
        exit(EXIT_FAILURE);
    }

    status_list_t *status_list = init_status_list(memsize);
    status_list = update_status(status_list, memory_list, memsize);
    long long position;           // where to be allocated
    long long evicted_count = 0;  // whether processes have been evicted
    long long *evicted_add = (long long*)malloc(memsize * sizeof(long long));  // evicted addresses
    
    // evict processes until has enough space
    while ((position = has_enough(status_list, proc->mem_req)) == -1) {
        // find earliest proc to remove
        // find the first value and use it as the init value for min
        long long min_time = -1, min_proc = -1;
        for (long long i = 0; i < memsize; i++) {
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

    sort_evicted_add(evicted_add, evicted_count);

    // print evict message
    if (evicted_count > 0) {
        // convert evicted add to string
        char *msg = (char*)malloc(MAX_MSG_LEN * sizeof(char));
        strcpy(msg, EVICTED_MSG);
        strcat(msg, list_to_str(evicted_add, evicted_count));

        // print messsage
        print_status(time, EVICTED, -1, msg);  // no proc id needed

        free(msg);
    }

    for (long long i = position; i < position + proc->mem_req; i++) {
        memory_list[i].proc_id = proc->id;
        memory_list[i].time_used = time;
    }

    status_list = update_status(status_list, memory_list, memsize);

}

// return how many empty pages are in the memory
long long num_empty(unit_t *memory_list, long long memsize) {
    long long count = 0;
    for (long long i = 0; i < memsize; i++) {
        if (memory_list[i].proc_id == HOLE) {
            count++;
        }
    }
    return count;
}

// check if process is in memory for virtual memory
// return page count of the process
long long proc_page_count(unit_t *mem, long long size, long long proc_id) {
    long long count = 0;
    for (long long i = 0; i < size; i++) {
        if (mem[i].proc_id == proc_id) {
            count++;
        }
    }
    return count;
}

// evict a page, and return the page number
long long evict_page(unit_t *memory_list, long long memsize, long long proc_id) {
    for (long long i = 0; i < memsize; i++) {
        if (memory_list[i].proc_id == proc_id) {
            memory_list[i].proc_id = HOLE;
            return i;
        }
    }
    // page not found
    return -1;
}

// virtual memory stuff, return extra time
long long virt_mem(unit_t *memory_list, long long memsize, process_t *proc, long long time) {

    if ((memory_list == NULL) || (proc == NULL)) {
        perror("ERROR allocating process to memory");
        exit(EXIT_FAILURE);
    }
    
    // calculate how many page to allocate
    long long empty_count = num_empty(memory_list, memsize);
    long long extra_time = 0;
    long long evicted_count = 0, *evicted_add = (long long*)malloc(memsize * sizeof(long long));

    long long page_count = proc_page_count(memory_list, memsize, proc->id);
    long long page_req = PAGE_NEEDED_V > proc->mem_req ? proc->mem_req : PAGE_NEEDED_V;

    // if theres no free pages
    if (empty_count == 0) {
        // if proc in memory
        if (page_count >= page_req) {  // no free page, but proc in memory
            // page fault
            if (proc->mem_req > page_count) {
                proc->remaining_time += proc->mem_req - page_count;
            }
        } else {  // no free page and proc not (enough) yet in memory
            // evict pages until enough
            long long page_in_mem = proc_page_count(memory_list, memsize, proc->id);
            while (num_empty(memory_list, memsize) < page_req - page_in_mem) {
                // find min
                long long min_time = -1, min_proc = -1;
                for (long long i = 0; i < memsize; i++) {
                    // this memory is occupied
                    if ((memory_list[i].proc_id != -1) && (memory_list[i].proc_id != proc->id)) {
                        if ((min_time == -1)  || (memory_list[i].time_used < min_time)) {
                            min_time = memory_list[i].time_used;
                            min_proc = memory_list[i].proc_id;
                        }
                    }
                }
                evicted_add[evicted_count] = evict_page(memory_list, memsize, min_proc);
                evicted_count++;
            }

            sort_evicted_add(evicted_add, evicted_count);

            // print evict message
            if (evicted_count > 0) {
                char *msg = (char*)malloc(MAX_MSG_LEN * sizeof(char));
                strcpy(msg, EVICTED_MSG);
                strcat(msg, list_to_str(evicted_add, evicted_count));
                // print messsage
                print_status(time, EVICTED, -1, msg);  // no proc id needed
                free(msg);
            }

            // allocate
            for (long long i = 0; i < memsize; i++) {
                if (memory_list[i].proc_id == HOLE) {
                    memory_list[i].proc_id = proc->id;
                    memory_list[i].time_used = time;
                    extra_time += TIME_PER_PAGE;
                }
            }

            // add page fault time
            page_count = proc_page_count(memory_list, memsize, proc->id);
            if (proc->mem_req > page_count) {
                proc->remaining_time += proc->mem_req - page_count;
            }

        }  // end of - no free page, process not (enough) in memory 

    } else {  // if there are free pages
        if (page_count >= page_req) {  // has enough page to run
            // fill in empty page
            for (long long i = 0; i < memsize; i++) {
                // if reaches requirement
                if ((page_count = proc_page_count(memory_list, memsize, proc->id)) >= proc->mem_req) {
                    break;
                }
                // allocate
                if (memory_list[i].proc_id == HOLE) {
                    memory_list[i].proc_id = proc->id;
                    memory_list[i].time_used = time;
                    extra_time += TIME_PER_PAGE;
                }
            }
            // add page fault time
            page_count = proc_page_count(memory_list, memsize, proc->id);
            if (proc->mem_req > page_count) {
                proc->remaining_time += proc->mem_req - page_count;
            }

        } else {  // less than required amount of pages in memory
            // release first
            long long page_in_mem = proc_page_count(memory_list, memsize, proc->id);
            while (num_empty(memory_list, memsize) < page_req - page_in_mem) {
                // find min
                long long min_time = -1, min_proc = -1;
                for (long long i = 0; i < memsize; i++) { 
                    // this memory is occupied
                    if ((memory_list[i].proc_id != -1) && (memory_list[i].proc_id != proc->id)) {
                        if ((min_time == -1)  || (memory_list[i].time_used < min_time)) {
                            min_time = memory_list[i].time_used;
                            min_proc = memory_list[i].proc_id;
                        }
                    }
                }
                evicted_add[evicted_count] = evict_page(memory_list, memsize, min_proc);
                evicted_count++;
            }
            
            sort_evicted_add(evicted_add, evicted_count);

            // print evict message
            if (evicted_count > 0) {
                char *msg = (char*)malloc(MAX_MSG_LEN * sizeof(char));
                strcpy(msg, EVICTED_MSG);
                strcat(msg, list_to_str(evicted_add, evicted_count));
                // print messsage
                print_status(time, EVICTED, -1, msg);  // no proc id needed
                free(msg);
            }
        
            // allocate as many as possible
            // FIXME: added another mem at the end
            for (long long i = 0; i < memsize; i++) {
                // if reaches requirement
                if ((page_count = proc_page_count(memory_list, memsize, proc->id)) >= proc->mem_req) {
                    break;
                }
                // allocate
                if (memory_list[i].proc_id == HOLE) {
                    memory_list[i].proc_id = proc->id;
                    memory_list[i].time_used = time;
                    extra_time += TIME_PER_PAGE;
                }
            }

            // add page fault time
            page_count = proc_page_count(memory_list, memsize, proc->id);
            if (proc->mem_req > page_count) {
                proc->remaining_time += proc->mem_req - page_count;
            }

        }
    }

    return extra_time;
    
}