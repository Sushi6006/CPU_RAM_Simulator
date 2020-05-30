#ifndef _memory_h
#define _memory_h

#include "processlist.h"
#include "memory.h"

#define PAGE_SIZE 4
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

int has_enough(status_list_t *status_list, int req_mem);
status_t *create_status(int status, int start, int size);
status_list_t *add_status(status_list_t *list, status_t *new_status);
status_list_t *update_status(unit_t *memory_list, int len);

status_list_t *allocate_proc(unit_t *memory_list, int memsize, status_list_t *status_list, process_t *proc, int time);
void evict_proc(unit_t *memory_list, int memsize, int proc_id);
// void *finish_proc(unit_t *memory_list, int memsize, int proc_id);

#endif