#ifndef _processlist_h
#define _processlist_h

// a process
typedef struct Process process_t;
struct Process {
    int arrival_time;
    int id;
    int mem_req;
    int job_time;
    int status;
    process_t *next;
};
// a (linked) list of processes
typedef struct ProcessList {
    int process_count;
    process_t *head_process;
    process_t *foot_process;
} process_list_t;

process_list_t *init_process_list();
process_list_t *add_process(process_list_t *list, int arrival_time, int id, int mem_req, int job_time);

int compare(process_t *node1, process_t *node2);
void swap(process_t *node1, process_t *node2);
void sort(process_list_t *list);

#endif