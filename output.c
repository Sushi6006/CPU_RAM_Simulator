#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "output.h"
#include "scheduler.h"
#include "processlist.h"
#include "memory.h"

// print out the status when changed
void print_status(int time, int status, int proc_id, char *msg) {
    switch (status) {
        case RUNNING:
            printf("%d, RUNNING, id=%d, %s\n", time, proc_id, msg);
            break;
        case FINISHED:
            printf("%d, FINISHED, id=%d, %s\n", time, proc_id, msg);
            break;
        case EVICTED:
            printf("%d, EVICTED, %s\n", time, msg);
        default:
            break;
    }
}

// print stats at the end
void print_stats(int process_executed, int total_turnaround, float total_overhead, float max_overhead, int time,
                 int avg_throughput, int min_throughput, int max_throughput) {
    printf("Throughput %d, %d, %d\n", avg_throughput, min_throughput, max_throughput);
    printf("Turnaround time %d\n", (int)ceil((double)total_turnaround / (double)process_executed));
    printf("Time overhead %.2f %.2f\n", max_overhead, total_overhead / process_executed);
    printf("Makespan %d\n", time);
}

// print all the specs given by command line
void print_spec(char* file_name, spec_t spec) {

    int sch_algo = spec.sch_algo;
    int mem_allo = spec.mem_allo;
    int mem_size = spec.mem_size;
    int quantum = spec.quantum;

    char sch_algo_name[MAX_NAME_LEN], mem_allo_name[MAX_NAME_LEN];

    // name of scheduling algorithm
    if (sch_algo == SCH_FF) {
        strcpy(sch_algo_name, NAME_FF);
    } else if (sch_algo == SCH_RR) {
        strcpy(sch_algo_name, NAME_RR);
    } else {
        strcpy(sch_algo_name, NAME_C);
    }
    // name of memory allocation
    if (mem_allo == MEM_P) {
        strcpy(mem_allo_name, NAME_P);
    } else if (mem_allo == MEM_U) {
        strcpy(mem_allo_name, NAME_U);
    } else if (mem_allo == MEM_V) {
        strcpy(mem_allo_name, NAME_V);
    } else {
        strcpy(mem_allo_name, NAME_C);
    }

    printf("========== PROGRAM SPEC ==========\n");
    printf("Filename:             %s\n", file_name);
    printf("Scheduling Algorithm: %s\n", sch_algo_name);
    printf("Memory Allocation:    %s\n", mem_allo_name);
    printf("Memory Size:          %d\n", mem_size);
    printf("Quantum:              %d\n", quantum);

}

// print information of one process
void print_process(process_t *process) {
    if (process == NULL) {
        perror("ERROR printing null process");
        exit(EXIT_FAILURE);
    }
    printf("===== PROCESS %2d =====\n", process->id);
    printf("  Arrival Time: %d\n", process->arrival_time);
    printf("  Job Time:     %d\n", process->job_time);
    printf("  Mem Required: %d\n", process->mem_req);
    printf("  Remain Time:  %d\n\n", process->remaining_time);
}

// print the whole list of processes
void print_process_list(process_list_t *process_list) {

    if (process_list == NULL) {
        perror("ERROR printing empty list");
        exit(EXIT_FAILURE);
    }

    process_t *current_process = process_list->head_process;
    while (current_process != NULL) {
        print_process(current_process);
        current_process = current_process->next;
    }
}

// print information of a unit of memory
void print_memory(unit_t mem_unit, int mem_id) {
    printf("%2d. === proc_id:   %d\n", mem_id, mem_unit.proc_id);
    // printf("    === time_used: %d\n", mem_unit.time_used);
}

// print list of memory
void print_memory_list(unit_t *mem_list, int size) {
    for (int i = 0; i < size; i++) {
        print_memory(mem_list[i], i);
    }
}

// print information of one status node
void print_status_node(status_t *status) {
    if (status == NULL) {
        perror("ERROR printing null status node");
        exit(EXIT_FAILURE);
    }
    printf("===== STATUS %2d =====\n", status->status);
    printf("  Start:  %d\n", status->start);
    printf("  Size:   %d\n", status->size);
}

// print the whole list of status
void print_status_list(status_list_t *list) {

    if (list == NULL) {
        perror("ERROR printing empty list");
        exit(EXIT_FAILURE);
    }

    status_t *curr_status = list->head;
    while (curr_status != NULL) {
        print_status_node(curr_status);
        curr_status = curr_status->next;
    }
}

