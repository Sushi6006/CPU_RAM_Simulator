#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "scheduler.h"
#include "dbgoutput.h"

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