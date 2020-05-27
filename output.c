#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "output.h"
#include "scheduler.h"
#include "processlist.h"

// print out the status when changed
void print_status(int time, process_t *process, int proc_remain) {
    switch (process->status) {
        case RUNNING:
            printf("%d, RUNNING, id=%d, remaining-time=%d\n", time, process->id, process->remaining_time);
            break;
        case FINISHED:
            printf("%d, FINISHED, id=%d, proc-remaining=%d\n", time, process->id, proc_remain);
            break;
        default:
            break;
    }
}

// print stats at the end
void print_stats(int process_executed, int total_turnaround, float total_overhead, float max_overhead, int time,
                 int avg_throughput, int min_throughput, int max_throughput) {
    printf("Throughput %d, %d, %d\n", avg_throughput, min_throughput, max_throughput);
    printf("Turnaround time %d\n", (int)ceil((float)total_turnaround / (float)process_executed));
    printf("Time overhead %.2f %.2f\n", max_overhead, total_overhead / process_executed);
    printf("Makespan %d\n", time);
}

// print all the specs given by command line
void print_spec(char* file_name, int sch_algo, int mem_allo, int mem_size, int quantum) {

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
void print_process(process_t process) {
    printf("===== PROCESS %2d =====\n", process.id);
    printf("  Arrival Time: %d\n", process.arrival_time);
    printf("  Job Time:     %d\n", process.job_time);
    printf("  Mem Required: %d\n\n", process.mem_req);
}

void print_process_list(process_list_t *process_list) {
    process_t *current_process = process_list->head_process;
    while (current_process != NULL) {
        print_process(*current_process);
        current_process = current_process->next;
    }
}