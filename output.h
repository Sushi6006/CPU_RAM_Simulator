#ifndef _dbgoutput_h
#define _dbgoutput_h

#include "processlist.h"
#include "memory.h"

#define MAX_NAME_LEN 32  // max length of a name
#define MAX_MSG_LEN 64   // max length of a custom message

// names of algorithms/implementations given
#define NAME_FF "FIRST-COME FIRST-SERVED"
#define NAME_RR "ROBIN-ROUND"
#define NAME_P  "SWAPPING"
#define NAME_V  "VIRTUAL MEMORY"
#define NAME_U  "UNLIMITED MEMORY"
#define NAME_C  "BYO"

// messages to be printed out
#define RUNNING_MSG "remaining-time=%d"
#define EVICTED_MSG "mem-addresses=<%s>"
#define FINISHED_MSG "proc-remaining=%d"


// for required output
void print_status(int time, process_t *process, char *msg);
void print_stats(int process_executed, int total_turnaround, float total_overhead, float max_overhead, int time,
                 int avg_throughput, int min_throughput, int max_throughput);

// debug uses
void print_spec(char* file_name, int sch_algo, int mem_allo, int mem_size, int quantum);
void print_process(process_t *process);
void print_process_list(process_list_t *process_list);
void print_memory(unit_t mem_unit, int mem_id);
void print_memory_list(unit_t *mem_list, int size);
void print_status_node(status_t *status);
void print_status_list(status_list_t *list);

#endif