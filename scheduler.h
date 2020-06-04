#ifndef _scheduler_h
#define _scheduler_h

#include "processlist.h"
#include "memory.h"

#define MAX_FILENAME  128   // max length for a file name
#define MAX_SCHE_ALGO 4     // max length for the name of the algorithm
#define MAX_PROC_LINE 32    // max line of one process (in file)

#define DEFAULT_QUANTUM 10  // default quantum

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
#define NOT_READY -1  // havent encountered the process
#define ARRIVED 0     // never excuted
#define RUNNING 1     // currently running
// skipped 2 because i had "PAUSED" but didn't end up using it
#define EVICTED 3     // evicted
#define FINISHED 4    // finished

#define THROUGHPUT_TIMEFRAME 60.0

typedef struct Specifications {
    int sch_algo;    // scheduling algorithm
    int mem_allo;    // memory management
    long long mem_size;    // size of memory
    long long quantum;     // quantum
    long long proc_count;  // number of processes
} spec_t;

// functions for input
void read_args(int argc, char *argv[], char* file_name, spec_t *spec);
process_list_t *read_process(char *file_name);

// does stuff with single process
long long run_proc(process_t *proc, long long time, unit_t *memory_list, spec_t spec);
void finish_proc(process_t *proc, long long time, unit_t *memory_list, spec_t spec, long long arrived_count, long long *executed_count);

// cpu scheduling
void schedule(process_list_t *process_list, unit_t *memory_list, spec_t spec);
void fcfs(process_list_t *process_list, unit_t *memory_list, spec_t spec);
void rr(process_list_t *process_list, unit_t *memory_list, spec_t spec);
void sjf(process_list_t *process_list, unit_t *memory_list, spec_t spec);

void calc_stats(long long *tot_turnaround, float *max_overhead, float *tot_overhead, long long time, process_t *proc);
void free_lists(process_list_t *proc_list, unit_t *mem_list);
#endif