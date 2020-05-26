#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "scheduler.h"
#include "dbgoutput.h"
#include "processlist.h"

int main(int argc, char *argv[]) {

    // initialise all the variables
    char *file_name = (char*)malloc(sizeof(char) * MAX_FILENAME);  // the file name
    int sch_algo;  // indicate the scheduling algorithm
    int mem_allo;  // indicate the memory allocation method
    int mem_size;  // size of memory given
    int quantum;
    sch_algo = mem_allo = mem_size = quantum = UNSPECIFIED;  // init value

    // parse command line arguments
    read_args(argc, argv, file_name, &sch_algo, &mem_allo, &mem_size, &quantum);
    // read & store processes
    process_list_t *process_list = read_process(file_name);
    free(file_name);
    // sort the processes
    sort(process_list);

    print_process_list(process_list);

    // TODO: CPU
    return 0;
}

// parse all the args given
void read_args(int argc, char *argv[], char* file_name, int *sch_algo, int *mem_allo, int *mem_size, int *quantum) {
    // reading flags
    int c;
    while ((c = getopt(argc, argv, "f:a:m:s:q:"))) {
        int getopt_ended = 0;
        switch (c) {
            case FLAG_FILE:
                sprintf(file_name, "%s", optarg);
                break;
            case FLAG_SCH_ALGO:
                (*sch_algo) = optarg[0];
                break;
            case FLAG_MEM_ALLO:
                (*mem_allo) = optarg[0];
                break;
            case FLAG_MEM_SIZE:
                (*mem_size) = atoi(optarg);
                break;
            case FLAG_QUANTUM:
                (*quantum) = atoi(optarg);
                break;
            default:
                getopt_ended = 1;  // finished reading the flags
        }

        if (getopt_ended) break;
    }

    // print out specs parsed
    // print_spec(file_name, *sch_algo, *mem_allo, *mem_size, *quantum);
}

// read from file
process_list_t *read_process(char *file_name) {

    // init the list
    process_list_t *process_list = init_process_list();
    
    // open the file in read mode
    FILE *process_file = fopen(file_name, "r");
    if (process_file == NULL) {
        perror("ERROR opening file");
        exit(EXIT_FAILURE);
    }

    // read processes line by line
    char *line = (char*)malloc(MAX_PROC_LINE * sizeof(char));
    size_t len; ssize_t read;
    while ((read = getline(&line, &len, process_file)) != -1) {
        int arrival_time, id, mem_size, job_time;
        sscanf(line, "%d%d%d%d", &arrival_time, &id, &mem_size, &job_time);
        process_list = add_process(process_list, arrival_time, id, mem_size, job_time);
    }

    // print_process_list(process_list);

    fclose(process_file);
    free(line);

    return process_list;
}

// select 
void select_algo(process_list_t *process_list, int quantum, int sch_algo) {
    switch (sch_algo) {
        case SCH_FF:
            fcfs(process_list);
            break;
        case SCH_RR:
            rr(process_list, quantum);
            break;
        case BYO:
            cs(process_list);
            break;
        default:
            perror("ERROR selecting scheduling algorithm");
            exit(EXIT_FAILURE);
    }
}

// first-come first-served
void fcfs(process_list_t *process_list) {
    
}