#ifndef _scheduler_h
#define _scheduler_h

#include "processlist.h"

#define MAX_FILENAME  128  // max length for a file name
#define MAX_SCHE_ALGO 4    // max length for the name of the algorithm
#define MAX_PROC_LINE 32   // max line of one process (in file)

// define all the flags
#define FLAG_FILE 'f'      // file name
#define FLAG_SCH_ALGO 'a'  // scheduling algorithm
#define FLAG_MEM_ALLO 'm'  // memory allocation
#define FLAG_MEM_SIZE 's'  // memory size
#define FLAG_QUANTUM 'q'   // quantum

// defines all the flag args for corresponding algos
#define UNSPECIFIED -1  // default value when no flags were read
#define SCH_FF 'f'      // first-come first-served
#define SCH_RR 'r'      // robin-round
#define MEM_U 'u'       // unlimited memory
#define MEM_P 'p'       // swapping
#define MEM_V 'v'       // virtual memory
#define BYO 'c'         // tbd

// define states of processes
#define RUNNING 1
#define FINISHED 0

// functions for input
void read_args(int argc, char *argv[],
               char* file_name, int *sch_algo, int *mem_allo, int *mem_size, int *quantum);
process_list_t *read_process(char *file_name);

// cpu scheduling
void select_algo(process_list_t *process_list, int quantum, int sch_algo);
void fcfs(process_list_t *process_list);
void rr(process_list_t *process_list, int quantum);
void cs(process_list_t *process_list);

// memory allocation
void virtual_mem();
void swapping();
void cm();

#endif