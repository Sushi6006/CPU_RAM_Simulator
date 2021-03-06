#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "scheduler.h"
#include "processlist.h"
#include "memory.h"
#include "output.h"

int main(int argc, char *argv[]) {

    // initialise all the variables
    char *file_name = (char*)malloc(sizeof(char) * MAX_FILENAME);  // the file name
    spec_t spec;
    spec.sch_algo = spec.mem_allo = spec.mem_size = UNSPECIFIED;
    spec.quantum = DEFAULT_QUANTUM;

    // read & store processes and other info
    read_args(argc, argv, file_name, &spec);
    process_list_t *process_list = read_process(file_name);
    free(file_name);
    unit_t *memory_list;
    if (spec.mem_size != -1) {  // not unlimited
        memory_list = init_memory_list(spec.mem_size);
    } else {
        // still need to allocate, otherwise
        // gets terminated abnormally
        memory_list = init_memory_list(1);
    }
    spec.proc_count = process_list->process_count;
    

    // enter cpu scheduling
    sort(process_list);  // sort by proc_id when the arrival time is the same
    schedule(process_list, memory_list, spec);

    // free
    free_lists(process_list, memory_list);
    
    // yay
    return 0;
}

// parse all the args given
void read_args(int argc, char *argv[], char* file_name, spec_t *spec) {
    // reading flags
    int c;
    while ((c = getopt(argc, argv, "f:a:m:s:q:"))) {
        int getopt_ended = 0;
        switch (c) {
            case FLAG_FILE:
                sprintf(file_name, "%s", optarg);
                break;
            case FLAG_SCH_ALGO:
                (*spec).sch_algo = optarg[0];
                break;
            case FLAG_MEM_ALLO:
                (*spec).mem_allo = optarg[0];
                break;
            case FLAG_MEM_SIZE:
                if (optarg[0] == 'u') {
                    (*spec).mem_size = UNSPECIFIED;  // unlimited
                } else {
                    (*spec).mem_size = atoi(optarg) / 4;  // converge to page count
                }
                break;
            case FLAG_QUANTUM:
                (*spec).quantum = atoi(optarg);
                break;
            default:
                getopt_ended = 1;  // finished reading the flags
        }

        if (getopt_ended) break;
    }
}

// read from file
process_list_t *read_process(char *file_name) {

    // init the list
    process_list_t *process_list = init_process_list();
    
    // open the file in read mode
    FILE *process_file = fopen(file_name, "r");
    if (process_file == NULL) {
        perror("ERROR opening file");
        exit(EXIT_FAILURE);
    }

    // read processes line by line
    char *line = (char*)malloc(MAX_PROC_LINE * sizeof(char));
    size_t len; ssize_t read;
    while ((read = getline(&line, &len, process_file)) != EOF) {
        long long arrival_time, id, mem_req, job_time;
        sscanf(line, "%lld%lld%lld%lld", &arrival_time, &id, &mem_req, &job_time);
        process_list = add_process(process_list, create_process(arrival_time, id, mem_req / 4, job_time));
    }

    fclose(process_file);
    free(line);

    return process_list;
}

// run the process, returns extra time taken
long long run_proc(process_t *proc, long long time, unit_t *memory_list, spec_t spec) {

    long long extra_time = 0;  // extra time took to run the process

    proc->status = RUNNING;
    char *msg = (char*)malloc(MAX_MSG_LEN * sizeof(char));
    char *msg2 = (char*)malloc(MAX_MSG_LEN * sizeof(char));
    strcpy(msg2, "");

    // set time for memory
    if (spec.mem_size != UNSPECIFIED) {
        switch (spec.mem_allo) {
            case MEM_P:
                // Swapping
                if (!mem_has_proc(memory_list, spec.mem_size, proc->id)) {
                    swap_mem(memory_list, spec.mem_size, proc, time);   
                    extra_time = proc->mem_req * TIME_PER_PAGE;
                }
                break;
            case  MEM_V:
                // Virtual Memory
                extra_time = virt_mem(memory_list, spec.mem_size, proc, time);
                break;
            case BYO:
                // CM
                extra_time = cm(memory_list, spec.mem_size, proc, time);
                break;
            default:
                perror("ERROR selecting memory allocation");
                exit(EXIT_FAILURE);
        }

        // calculates and prepare output message
        long long *proc_mem = (long long*)malloc(spec.mem_size * sizeof(long long) + 1);
        long long mem_usage = calc_mem_usage(memory_list, spec.mem_size);
        int usage_percentage = (int)ceil((double)mem_usage / (double)spec.mem_size * 100.0);
        long long proc_usage = calc_proc_usage(memory_list, spec.mem_size, proc_mem, proc->id, time);
        sprintf(msg2, RUNNING_MSG2, extra_time, usage_percentage, list_to_str(proc_mem, proc_usage));
    }

    // add second part of message if required (limited memory)
    sprintf(msg, RUNNING_MSG, proc->remaining_time);
    strcat(msg, msg2);
    // print running message
    print_status(time, proc->status, proc->id, msg);
    free(msg);

    return extra_time;
}

// finish up the prcoess
void finish_proc(process_t *proc, long long time, unit_t *memory_list, spec_t spec, long long arrived_count, long long *executed_count) {
    
    proc->status = FINISHED;
    (*executed_count)++;

    char *msg = (char*)malloc(MAX_MSG_LEN * sizeof(char));
    sprintf(msg, FINISHED_MSG, arrived_count - (*executed_count));

    // evict memory if needed (limited memory)
    if (spec.mem_size != UNSPECIFIED) {
        // free memory allocated to process
        long long *evicted_add = (long long*)malloc(spec.mem_size * sizeof(long long));
        long long evicted_count = 0;
        evict_proc(memory_list, spec.mem_size, proc->id, evicted_add, &evicted_count);
        // print message
        char *msg2 = (char*)malloc(MAX_MSG_LEN * sizeof(char));
        strcpy(msg2, EVICTED_MSG);
        strcat(msg2, list_to_str(evicted_add, evicted_count));
        print_status(time, EVICTED, HOLE, msg2);  // no proc id needed
        free(msg2);
    }

    print_status(time, proc->status, proc->id, msg);
    free(msg);
}

// select algo for the simulationw
void schedule(process_list_t *process_list, unit_t *memory_list, spec_t spec) {
    switch (spec.sch_algo) {
        case SCH_FF:
            // First-Come First-Served
            fcfs(process_list, memory_list, spec);
            break;
        case SCH_RR:
            // Round-Robin
            rr(process_list, memory_list, spec);
            break;
        case BYO:
            // Shortest Job First / Shortest Process Next
            sjf(process_list, memory_list, spec);
            break;
        default:
            perror("ERROR selecting scheduling algorithm");
            exit(EXIT_FAILURE);
    }
}

// find the number of processes arrived before time x
long long num_arrived(process_list_t *proc_list, long long time) {
    if (proc_list == NULL) {
        perror("ERROR finding arrived process in null list");
        exit(EXIT_FAILURE);
    }

    process_t *curr_proc = proc_list->head_process;
    long long count = 0;
    while (curr_proc != NULL) {
        if (curr_proc->arrival_time <= time) {
            count++;
        } else {
            break;
        }
        curr_proc = curr_proc->next;
    }

    return count;
}

// first-come first-served
void fcfs(process_list_t *process_list, unit_t *memory_list, spec_t spec) {

    if (process_list == NULL) {
        perror("ERROR scheduling empty list");
        exit(EXIT_FAILURE);
    }

    process_t *curr_process = process_list->head_process;  // current process
    long long time = 0;

    // for stats
    long long executed_count = 0;
    long long finish_times[spec.proc_count + 1];
    long long total_turnaround = 0;
    float max_overhead = 0, total_overhead = 0;

    // every loop is one process
    while (curr_process != NULL) {
        // if process arrives
        if (curr_process->arrival_time <= time) {
            // process started running
            time += run_proc(curr_process, time, memory_list, spec);
            time += curr_process->job_time;

            // process finished
            finish_times[executed_count] = time;
            finish_proc(curr_process, time, memory_list, spec, num_arrived(process_list, time), &executed_count);
            
            // calculate stats
            calc_stats(&total_turnaround, &max_overhead, &total_overhead, time, curr_process);
            
            // move on to the next process
            curr_process = curr_process->next;

        } else {
            time++;  // 1 unit of time pass by if no process arrives
        }

    }

    // calc & print stats
    long long min_tp = 61, max_tp = -1, total_tp = 0;
    for (long long i = 0; i < time; i += THROUGHPUT_TIMEFRAME) {
        long long count = 0;
        for (long long j = 0; j < executed_count; j++) {
            if ((finish_times[j] > i) && (finish_times[j] <= i + THROUGHPUT_TIMEFRAME)) {
                count++;
            }
        }
        if (count > max_tp) max_tp = count;
        if (count < min_tp) min_tp = count;
        total_tp += count;
    }
    long long avg_tp = (long long)ceil((double)executed_count / (ceil((double)time / THROUGHPUT_TIMEFRAME)));
    print_stats(executed_count, total_turnaround, total_overhead, max_overhead, time, avg_tp, min_tp, max_tp);
}

// robin round
void rr(process_list_t *process_list, unit_t *memory_list, spec_t spec) {

    if (process_list == NULL) {
        perror("ERROR scheduling empty list");
        exit(EXIT_FAILURE);
    }

    // init
    long long time = 0;
    long long quantum = spec.quantum;

    // for list to arrive
    process_list_t *arrived_list = init_process_list();
    process_t *curr_process, *arriving_process = process_list->head_process;
    long long executed_count = 0, arrived_count = 0;

    // for stats
    long long finish_times[spec.proc_count + 1];
    long long total_turnaround = 0;
    float max_overhead = 0, total_overhead = 0;

    // executing all processes
    while (executed_count != process_list->process_count) {

        // let process arrive
        arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);

        // run the first in queue
        curr_process = arrived_list->head_process;
        if (curr_process == NULL) {
            // no process arrived
            time++;
            continue;
        }
        time += run_proc(curr_process, time, memory_list, spec);

        // cannot finish within quantum
        if (curr_process->remaining_time > quantum) {  
            time += quantum;
            // let process arrive before moving curr proc to the end
            arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);
            curr_process->remaining_time -= quantum;
            // move process to end of queue
            arrived_list = move_proc_to_end(arrived_list, curr_process);
        } else {  // can finish within quantum
            time += curr_process->remaining_time;
            // finishing process
            arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);
            curr_process->remaining_time = 0;
            finish_times[executed_count] = time;
            finish_proc(curr_process, time, memory_list, spec, num_arrived(process_list, time), &executed_count);

            // calculate stats
            calc_stats(&total_turnaround, &max_overhead, &total_overhead, time, curr_process);
            
            // remove finished process
            arrived_list = delete_head_proc(arrived_list);
        }
    }

    // calc & print stats
    long long min_tp = 61, max_tp = -1;
    for (long long i = 0; i < time; i += THROUGHPUT_TIMEFRAME) {
        long long count = 0;
        for (long long j = 0; j < executed_count; j++) {
            if ((finish_times[j] > i) && (finish_times[j] <= i + THROUGHPUT_TIMEFRAME)) {
                count++;
            }
        }
        if (count > max_tp) max_tp = count;
        if (count < min_tp) min_tp = count;
    }
    long long avg_tp = (long long)ceil((double)executed_count / (ceil((double)time / THROUGHPUT_TIMEFRAME)));
    print_stats(executed_count, total_turnaround, total_overhead, max_overhead, time, avg_tp, min_tp, max_tp);
}

// shortest job first / shortest process next (spn)
void sjf(process_list_t *process_list, unit_t *memory_list, spec_t spec) {

    if (process_list == NULL) {
        perror("ERROR scheduling empty list");
        exit(EXIT_FAILURE);
    }
    
    long long time = 0;

    // for processes to arrive
    process_list_t *arrived_list = init_process_list();
    process_t *curr_process, *min_process;
    process_t *arriving_process = process_list->head_process;
    
    // for stats
    long long executed_count = 0, arrived_count = 0;
    long long finish_times[spec.proc_count + 1];
    long long total_turnaround = 0;
    float max_overhead = 0, total_overhead = 0;

    while (executed_count < process_list->process_count) {
        // new processes arrive
        arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);
        
        if (arrived_list->head_process == NULL) {
            time++;  // no process arrived
            continue;
        }

        // find min and remove it
        curr_process = find_pre_min(arrived_list);
        if (curr_process != NULL) { 
            process_t *temp = curr_process->next;
            curr_process->next = temp->next;  // remove min_process from list
            if (curr_process->next == NULL) {
                arrived_list->foot_process = curr_process;
            }
            min_process = create_process(temp->arrival_time, temp->id,
                                         temp->mem_req, temp->job_time);
            free(temp);

        } else {  // head is min
            process_t *temp = arrived_list->head_process;
            min_process = create_process(temp->arrival_time, temp->id,
                                         temp->mem_req, temp->job_time);
            arrived_list = delete_head_proc(arrived_list);
        }

        // run the process
        time += run_proc(min_process, time, memory_list, spec);
        time += min_process->job_time;

        // finish the process
        finish_times[executed_count] = time;
        finish_proc(min_process, time, memory_list, spec, num_arrived(process_list, time), &executed_count);
        
        // calculate stats
        calc_stats(&total_turnaround, &max_overhead, &total_overhead, time, min_process);
    }

    // calc & print stats
    long long min_tp = 61, max_tp = -1;
    for (long long i = 0; i < time + THROUGHPUT_TIMEFRAME; i += THROUGHPUT_TIMEFRAME) {
        long long count = 0;
        for (long long j = 0; j < executed_count; j++) {
            if ((finish_times[j] > i) && (finish_times[j] <= i + THROUGHPUT_TIMEFRAME)) {
                count++;
            }
        }
        if (count > max_tp) max_tp = count;
        if (count < min_tp) min_tp = count;
    }
    long long avg_tp = (long long)ceil((double)executed_count / (ceil((double)time / THROUGHPUT_TIMEFRAME)));
    print_stats(executed_count, total_turnaround, total_overhead, max_overhead, time, avg_tp, min_tp, max_tp);
}

// calculate required stats
void calc_stats(long long *tot_turnaround, float *max_overhead, float *tot_overhead, long long time, process_t *proc) {

    // calculate turnaround
    long long turnaround = time - proc->arrival_time ;
    (*tot_turnaround) += turnaround;
    
    // calculate overhead
    float overhead = (double)turnaround / (double)proc->job_time;
    (*tot_overhead) += overhead;
    if (overhead > (*max_overhead)) (*max_overhead) = overhead;

}

// free a linked list
void free_lists(process_list_t *proc_list, unit_t *mem_list) {
    // free process list
    if (proc_list != NULL) {
        process_t *prev, *curr = proc_list->head_process;
        while (curr != NULL) {
            prev = curr;
            curr = curr->next;
            free(prev);
        }
        free(proc_list);
    }

    // free memory list
    free(mem_list);
}