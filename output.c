#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "output.h"
#include "scheduler.h"
#include "processlist.h"
#include "memory.h"

// convert a list into a string in "..., ..., ...]" form, yes without the first bracket
char *list_to_str(long long *list, long long size) {

    char *s = (char*)malloc(MAX_MSG_LEN * sizeof(char));
    strcpy(s, "");

    for (long long i = 0; i < size; i++) {
        char addr_str[ADDR_STR_LEN];
        if ((i != 0) && (list[i] == 0)) {
            // initialisation caused some 0 at the end
            // if it is not the first 0 in the list, ignore them
            s[strlen(s) - 1] = ']';  // replace the last comma with ]
            break;
        }
        sprintf(addr_str, i < size - 1 ? "%lld," : "%lld]", list[i]);
        strcat(s, addr_str);
    }

    return s;
}

// print out the status when changed
void print_status(long long time, int status, long long proc_id, char *msg) {
    switch (status) {
        case RUNNING:
            printf("%lld, RUNNING, id=%lld, %s\n", time, proc_id, msg);
            break;
        case FINISHED:
            printf("%lld, FINISHED, id=%lld, %s\n", time, proc_id, msg);
            break;
        case EVICTED:
            printf("%lld, EVICTED, %s\n", time, msg);
        default:
            break;
    }
}

// print stats at the end
void print_stats(long long process_executed, long long total_turnaround, float total_overhead, float max_overhead, long long time,
                 long long avg_throughput, long long min_throughput, long long max_throughput) {
    printf("Throughput %lld, %lld, %lld\n", avg_throughput, min_throughput, max_throughput);
    printf("Turnaround time %lld\n", (long long)ceil((double)total_turnaround / (double)process_executed));
    printf("Time overhead %.2f %.2f\n", max_overhead, total_overhead / process_executed);
    printf("Makespan %lld\n", time);
}

// print all the specs given by command line
void print_spec(char* file_name, spec_t spec) {

    int sch_algo = spec.sch_algo;
    int mem_allo = spec.mem_allo;
    long long mem_size = spec.mem_size;
    long long quantum = spec.quantum;

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
    printf("Memory Size:          %lld\n", mem_size);
    printf("Quantum:              %lld\n", quantum);

}

// print information of one process
void print_process(process_t *process) {
    if (process == NULL) {
        perror("ERROR printing null process");
        exit(EXIT_FAILURE);
    }
    printf("===== PROCESS %2lld =====\n", process->id);
    printf("  Arrival Time: %lld\n", process->arrival_time);
    printf("  Job Time:     %lld\n", process->job_time);
    printf("  Mem Required: %lld\n", process->mem_req);
    printf("  Remain Time:  %lld\n\n", process->remaining_time);
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
void print_memory(unit_t mem_unit, long long mem_id) {
    printf("%2lld. === proc_id:   %lld\n", mem_id, mem_unit.proc_id);
}

// print list of memory
void print_memory_list(unit_t *mem_list, long long size) {
    for (long long i = 0; i < size; i++) {
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
    printf("  Start:  %lld\n", status->start);
    printf("  Size:   %lld\n", status->size);
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

