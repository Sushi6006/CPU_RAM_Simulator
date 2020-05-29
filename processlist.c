#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "processlist.h"
#include "scheduler.h"
#include "output.h"

// create an empty (linked) list of processes
process_list_t *init_process_list() {
    process_list_t *process_list;
    process_list = (process_list_t*)malloc(sizeof(*process_list));
    if (process_list == NULL) {
        perror("ERROR creating linked list of processes");
        exit(EXIT_FAILURE);
    }
    process_list->head_process = process_list->foot_process = NULL;
    process_list->process_count = 0;

    return process_list;
}

// create a process (pointer) with information
process_t *create_process(int arrival_time, int id, int mem_req, int job_time) {
    
    process_t *new_process;
    new_process = (process_t*)malloc(sizeof(*new_process));
    if (new_process == NULL) {
        perror("ERROR null list or new process node");
        exit(EXIT_FAILURE);
    }

    new_process->arrival_time = arrival_time;
    new_process->id = id;
    new_process->mem_req = mem_req;
    new_process->job_time = job_time;
    new_process->status = NOT_READY;
    new_process->remaining_time = job_time;
    new_process->next = NULL;

    return new_process;
}

// insert a node at the end of the process list
process_list_t *add_process(process_list_t *list, process_t *new_process) {

    if ((list == NULL) || (new_process == NULL)) {
        perror("ERROR adding process to list");
        exit(EXIT_FAILURE);
    }

    // add to the list
    list->process_count++;
    if (list->foot_process == NULL) {
        // first process
        list->head_process = list->foot_process = new_process;
    } else {
        list->foot_process->next = new_process;
        list->foot_process = new_process;
    }

    return list;
}

// delete head process
process_list_t *delete_head_proc(process_list_t *list) {

    if (list == NULL) {
        perror("ERROR removing process from list");
        exit(EXIT_FAILURE);
    }

    process_t *head_proc = list->head_process;
    list->head_process = head_proc->next;
    free(head_proc);

    return list;
}

// move a process to the end
process_list_t *move_proc_to_end(process_list_t *list, process_t *process) {
    if ((list == NULL) || (process == NULL)) {
        perror("ERROR moving process to end of list");
        exit(EXIT_FAILURE);
    }

    list->head_process = process->next;
    process->next = NULL;
    return add_process(list, process);
}

// processes arrive, used by rr
process_list_t *proc_arrive(process_t *arriving_proc, process_list_t *arrived_list, int time, int *arrived_count) {
    if (arriving_proc == NULL) {  // no more process to arrive
        return arrived_list;
    }
    if (arrived_list == NULL) {
        perror("ERROR adding process to null list");
        exit(EXIT_FAILURE);
    }
    while ((arriving_proc != NULL) && (arriving_proc->arrival_time <= time)) {
        // add to list only if it hasnt been added
        if (arriving_proc->status == NOT_READY) {
            // make a copy of arriving proc and add to `arrived list`
            arriving_proc->status = ARRIVED;
            (*arrived_count)++;
            arrived_list = add_process(arrived_list,
                                       create_process(arriving_proc->arrival_time, arriving_proc->id,
                                                      arriving_proc->mem_req, arriving_proc->job_time));
        }
        arriving_proc = arriving_proc->next;
    }

    return arrived_list;
}

// find the minimum and return the process ahead of it
// so the minimum process can be removed
process_t *find_pre_min(process_list_t *list) {

    if (list == NULL) {
        perror("ERROR finding minimum in empty list");
        exit(EXIT_FAILURE);
    }

    process_t *pre_min_proc = NULL;
    process_t *curr_proc = list->head_process;
    int min_jobtime = curr_proc->job_time;
    
    while (curr_proc->next != NULL) {
        if (curr_proc->next->job_time < min_jobtime) {
            pre_min_proc = curr_proc;
            min_jobtime = curr_proc->next->job_time;
        }
    }

    return pre_min_proc;

}

// compare two processes
// return 1 if node1 is greater, 0 if node2 is greater
int compare(process_t *node1, process_t *node2) {

    // make sure nodes are valid
    if ((node1 == NULL) || (node2 == NULL)) {
        perror("ERROR swapping two nodes");
        exit(EXIT_FAILURE);
    }

    // compare: arrival time first; if same, compare the id
    if (node1->arrival_time > node2->arrival_time) {
        return 1;
    } else if (node1->arrival_time < node2->arrival_time) {
        return 0;
    } else {
        return (node1->id > node2->id);
    }
}

// swap two nodes
void swap(process_t *node1, process_t *node2) {

    // make sure nodes are valid
    if ((node1 == NULL) || (node2 == NULL)) {
        perror("ERROR swapping two nodes");
        exit(EXIT_FAILURE);
    }

    int temp_arrival, temp_id, temp_mem, temp_jobtime, temp_status, temp_remaining;

    temp_arrival = node1->arrival_time;
    temp_id = node1->id;
    temp_mem = node1->mem_req;
    temp_jobtime = node1->job_time;
    temp_status = node1->status;
    temp_remaining = node1->remaining_time;

    node1->arrival_time = node2->arrival_time;
    node1->id = node2->id;
    node1->mem_req = node2->mem_req;
    node1->job_time = node2->job_time;
    node1->status = node2->status;
    node1->remaining_time = node2->remaining_time;

    node2->arrival_time = temp_arrival;
    node2->id = temp_id;
    node2->mem_req = temp_mem;
    node2->job_time = temp_jobtime;
    node2->status = temp_status;
    node1->remaining_time = temp_remaining;
}

// sort the list (bubble)
void sort(process_list_t *list) {

    // check for empty list
    if (list->head_process == NULL) {
        // no process, program terminates
        exit(EXIT_SUCCESS);
    }

    int swapped;
    process_t *ptr1, *lptr = NULL;
    process_t *head = list->head_process;
    do {
        swapped = 0;
        ptr1 = head;
        while (ptr1->next != lptr) {
            if (compare(ptr1, ptr1->next)) {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }

            ptr1 = ptr1->next;
        }
        lptr = ptr1;

    } while(swapped);
}