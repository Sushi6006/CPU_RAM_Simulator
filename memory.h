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

// a single unit (kb) of memory
typedef struct MemUnit {
    int proc_id;    // either process id or `HOLE` if empty
    int time_used;  // last time the memory was used
} unit_t;

// a node to record the occupied and empty blocks of memory
typedef struct MemStatus status_t;
struct MemStatus {
    int status;  // either `OCCU` or `HOLE`
    int start;   // start position of this continuous block
    int size;    // size of this block
    status_t *next;
};

// list to record the status of memory
typedef struct MemStatusList {
    status_t *head;
    status_t *foot;
} status_list_t;

typedef struct MemPage {
    int tbf;
    int tbf2;
} page_t;

unit_t *init_memory_list(int size);
status_list_t *init_status_list(int size);

int mem_has_proc(unit_t *mem, int size, int proc_id);
int mem_occupied_by_proc(unit_t *mem, int size, int *proc_mem, int proc_id);
int has_enough(status_list_t *status_list, int req_mem);
// FOR SWAPPING
status_t *create_status(int status, int start, int size);
status_list_t *add_status(status_list_t *list, status_t *new_status);
status_list_t *update_status(status_list_t *status_list, unit_t *memory_list, int len);
void evict_proc(unit_t *memory_list, int memsize, int proc_id, int *evicted_add, int *evicted_count);
void swap_mem(unit_t *memory_list, int memsize, process_t *proc, int time);

// FOR VIRTUAL MEMORY
int num_empty(unit_t *memory_list, int memsize);
int proc_page_count(unit_t *mem, int size, int proc_id);
int evict_page(unit_t *memory_list, int memsize, int proc_id);
int virt_mem(unit_t *memory_list, int memsize, process_t *proc, int time);

#endif