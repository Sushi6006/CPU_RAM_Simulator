#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "scheduler.h"
#include "dbgoutput.h"


int main(int argc, char *argv[]) {

    // initialise all the variables
    char *file_name;  // the file name
    int sch_algo;     // indicate the scheduling algorithm
    int mem_allo;     // indicate the memory allocation method
    int mem_size;     // size of memory given
    int quantum;

    sch_algo = mem_allo = mem_size = quantum = UNSPECIFIED;  // init

    // reading flags
    int c;
    while ((c = getopt(argc, argv, "f:a:m:s:q:"))) {
        int getopt_ended = 0;
        switch (c) {
            case FLAG_FILE:
                file_name = (char*)malloc(sizeof(char) * MAX_FILENAME);
                sprintf(file_name, "%s", optarg);
                break;
            case FLAG_SCH_ALGO:
                sch_algo = optarg[0];
                break;
            case FLAG_MEM_ALLO:
                mem_allo = optarg[0];
                break;
            case FLAG_MEM_SIZE:
                mem_size = atoi(optarg);
                break;
            case FLAG_QUANTUM:
                quantum = atoi(optarg);
                break;
            default:
                getopt_ended = 1;  // finished reading the flags
        }

        if (getopt_ended) break;
    }

    print_spec(file_name, sch_algo, mem_allo, mem_size, quantum);

    free(file_name);
    return 0;
}

