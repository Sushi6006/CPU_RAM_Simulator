#ifndef _dbgoutput_h
#define _dbgoutput_h

#include "processlist.h"

#define MAX_NAME_LEN 32  // max length of a name

// names of algorithms/implementations given
#define NAME_FF "FIRST-COME FIRST-SERVED"
#define NAME_RR "ROBIN-ROUND"
#define NAME_P  "SWAPPING"
#define NAME_V  "VIRTUAL MEMORY"
#define NAME_U  "UNLIMITED MEMORY"
#define NAME_C  "BYO"

void print_spec(char* file_name, int sch_algo, int mem_allo, int mem_size, int quantum);
void print_process(process_t process);
void print_process_list(process_list_t *process_list);

#endif