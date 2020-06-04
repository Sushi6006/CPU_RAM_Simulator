#ifndef _memory_h
#define _memory_h

#include "processlist.h"
#include "memory.h"

#define PAGE_SIZE 4
#define PAGE_NEEDED_V 4
#define ADDR_STR_LEN 8  // max length of a number(with comma) represeting an addr
#define TIME_PER_PAGE 2

#define HOLE -1  // indicate the block is not used by any process
#define OCCU 1   // occupied

// a single page of memory
typedef struct MemUnit {
    long long proc_id;    // either process id or `HOLE` if empty
    long long time_used;  // last time the memory was used
} unit_t;

// a node to record the occupied and empty blocks of memory
typedef struct MemStatus status_t;
struct MemStatus {
    int status;  // either `OCCU` or `HOLE`
    long long start;   // start position of this continuous block
    long long size;    // size of this block
    status_t *next;
};

// list to record the status of memory
typedef struct MemStatusList {
    status_t *head;
    status_t *foot;
} status_list_t;

unit_t *init_memory_list(long long size);
status_list_t *init_status_list(long long size);

int mem_has_proc(unit_t *mem, long long size, long long proc_id);
long long calc_mem_usage(unit_t *mem, long long size);
long long calc_proc_usage(unit_t *mem, long long size, long long *proc_mem, long long proc_id, long long time);
long long has_enough(status_list_t *status_list, long long req_mem);
void sort_evicted_add(long long *evicted_add, long long count);

// FOR SWAPPING
status_t *create_status(int status, long long start, long long size);
status_list_t *add_status(status_list_t *list, status_t *new_status);
status_list_t *update_status(status_list_t *status_list, unit_t *memory_list, long long len);
void evict_proc(unit_t *memory_list, long long memsize, long long proc_id, long long *evicted_add, long long *evicted_count);
void swap_mem(unit_t *memory_list, long long memsize, process_t *proc, long long time);

// FOR VIRTUAL MEMORY
long long num_empty(unit_t *memory_list, long long memsize);
long long proc_page_count(unit_t *mem, long long size, long long proc_id);
long long evict_page(unit_t *memory_list, long long memsize, long long proc_id);
long long virt_mem(unit_t *memory_list, long long memsize, process_t *proc, long long time);

#endif