#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "processlist.h"
#include "scheduler.h"
#include "dbgoutput.h"

// create an empty (linked) list of processes
process_list_t *init_process_list() {
    process_list_t *process_list;
    process_list = (process_list_t*)malloc(sizeof(*process_list));
    if (process_list == NULL) {
        perror("ERROR creating linked list of processes");
        exit(EXIT_FAILURE);
    }
    process_list->head_process = process_list->foot_process = NULL;

    return process_list;
}

// insert a node at the end of the process list
process_list_t *add_process(process_list_t *list, int arrival_time, int id, int mem_req, int job_time) {

    // init new process
    process_t *new_process;
    new_process = (process_t*)malloc(sizeof(*new_process));
    if ((list == NULL) || (new_process == NULL)) {
        perror("ERROR null list or new process node");
        exit(EXIT_FAILURE);
    }

    // assign values to the new process
    new_process->arrival_time = arrival_time;
    new_process->id = id;
    new_process->mem_req = mem_req;
    new_process->job_time = job_time;
    new_process->next = NULL;

    // add to the list
    if (list->foot_process == NULL) {
        // first process
        list->head_process = list->foot_process = new_process;
    } else {
        list->foot_process->next = new_process;
        list->foot_process = new_process;
    }

    return list;
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

    int temp_arrival, temp_id, temp_mem, temp_jobtime;

    temp_arrival = node1->arrival_time;
    temp_id = node1->id;
    temp_mem = node1->mem_req;
    temp_jobtime = node1->job_time;

    node1->arrival_time = node2->arrival_time;
    node1->id = node2->id;
    node1->mem_req = node2->mem_req;
    node1->job_time = node2->job_time;

    node2->arrival_time = temp_arrival;
    node2->id = temp_id;
    node2->mem_req = temp_mem;
    node2->job_time = temp_jobtime;
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