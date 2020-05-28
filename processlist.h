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
    int remaining_time;
    process_t *next;
};
// a (linked) list of processes
typedef struct ProcessList {
    int process_count;
    process_t *head_process;
    process_t *foot_process;
} process_list_t;

process_list_t *init_process_list();
process_t *create_process(int arrival_time, int id, int mem_req, int job_time);
process_list_t *add_process(process_list_t *list, process_t *new_process);
process_list_t *delete_head_proc(process_list_t *list, process_t *process);
process_list_t *move_proc_to_end(process_list_t *list, process_t *process);
process_list_t *proc_arrive(process_t *arriving_proc, process_list_t *arrived_list, int time, int *arrived_count);

int compare(process_t *node1, process_t *node2);
void swap(process_t *node1, process_t *node2);
void sort(process_list_t *list);

#endif