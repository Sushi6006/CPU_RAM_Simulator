#ifndef _memory_h
#define _memory_h

#define PAGE_SIZE 4
#define HOLE -1  // indicate the block is not used by any process

#include <processlist.h>

// a single unit (kb) of memory
typedef struct MemUnit {
    int proc_id;
    int time_used;
} unit_t;

// a node to record the occupied and empty blocks of memory
typedef struct MemStatus status_t;
struct MemStatus {
    int status;
    int start;
    int size;
    status_t *next;
};

// list to record the status of memory
typedef struct MemStatusList {
    status_t *head;
    status_t *foot;
} status_list_t;

typedef struct MemPage page_t;
typedef struct MemPage {
    int tbf;
};

unit_t *init_memory_list(int size);
status_list_t *init_status_list(int size);

int has_enough(status_list_t *status_list, int req_mem);
status_list_t *update_status(status_list_t *status_list, unit_t *memory_list);

status_list_t *allocate_proc(unit_t *memory_list, status_list_t *status_list, process_t *proc, int time);
status_list_t *evict_proc(unit_t *memory_list, status_list_t *status_list, int proc_id);
status_list_t *finish_proc(unit_t *memory_list, status_list_t *status_list, int proc_id);

#endif