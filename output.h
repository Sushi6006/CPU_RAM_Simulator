#ifndef _dbgoutput_h
#define _dbgoutput_h

#include "scheduler.h"
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
#define RUNNING_MSG "remaining-time=%lld"
#define RUNNING_MSG2 ", load-time=%lld, mem-usage=%d%%, mem-addresses=[%s"
#define EVICTED_MSG "mem-addresses=["
#define FINISHED_MSG "proc-remaining=%lld"


// for required output
char *list_to_str(long long *list, long long size);
void print_status(long long time, int status, long long proc_id, char *msg);
void print_stats(long long process_executed, long long total_turnaround, float total_overhead, float max_overhead, long long time,
                 long long avg_throughput, long long min_throughput, long long max_throughput);

// debug uses
void print_spec(char* file_name, spec_t spec);
void print_process(process_t *process);
void print_process_list(process_list_t *process_list);
void print_memory(unit_t mem_unit, long long mem_id);
void print_memory_list(unit_t *mem_list, long long size);
void print_status_node(status_t *status);
void print_status_list(status_list_t *list);

#endif