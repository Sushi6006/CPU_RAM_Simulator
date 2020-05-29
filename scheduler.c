#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "scheduler.h"
#include "output.h"
#include "processlist.h"

int main(int argc, char *argv[]) {

    // initialise all the variables
    char *file_name = (char*)malloc(sizeof(char) * MAX_FILENAME);  // the file name
    int sch_algo;  // indicate the scheduling algorithm
    int mem_allo;  // indicate the memory allocation method
    int mem_size;  // size of memory given
    int quantum = -1;
    sch_algo = mem_allo = mem_size = quantum = UNSPECIFIED;  // init value

    process_list_t *process_list;

    // read & store processes and other info
    read_args(argc, argv, file_name, &sch_algo, &mem_allo, &mem_size, &quantum);
    process_list = read_process(file_name);
    free(file_name);

    // enter cpu scheduling
    sort(process_list);
    select_algo(process_list, quantum, sch_algo);


    return 0;
}

// parse all the args given
void read_args(int argc, char *argv[], char* file_name, int *sch_algo, int *mem_allo, int *mem_size, int *quantum) {
    // reading flags
    int c;
    while ((c = getopt(argc, argv, "f:a:m:s:q:"))) {
        int getopt_ended = 0;
        switch (c) {
            case FLAG_FILE:
                sprintf(file_name, "%s", optarg);
                break;
            case FLAG_SCH_ALGO:
                (*sch_algo) = optarg[0];
                break;
            case FLAG_MEM_ALLO:
                (*mem_allo) = optarg[0];
                break;
            case FLAG_MEM_SIZE:
                (*mem_size) = atoi(optarg);
                break;
            case FLAG_QUANTUM:
                (*quantum) = atoi(optarg);
                break;
            default:
                getopt_ended = 1;  // finished reading the flags
        }

        if (getopt_ended) break;
    }

    // print out specs parsed
    // print_spec(file_name, *sch_algo, *mem_allo, *mem_size, *quantum);
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
    while ((read = getline(&line, &len, process_file)) != -1) {
        int arrival_time, id, mem_size, job_time;
        sscanf(line, "%d%d%d%d", &arrival_time, &id, &mem_size, &job_time);
        process_list = add_process(process_list, create_process(arrival_time, id, mem_size, job_time));
    }

    // print_process_list(process_list);

    fclose(process_file);
    free(line);

    return process_list;
}

// run the process
void run_proc(process_t *proc, int time) {
    proc->status = RUNNING;
    print_status(time, proc, -1);  // -1 as proc remain bc it wont be printed anyway
}

// finish up the prcoess
void finish_proc(process_t *proc, int time, int *executed_count, int total_proc) {
    proc->status = FINISHED;
    (*executed_count)++;
    print_status(time, proc, total_proc - (*executed_count));
}

// select algo for the simulation
void select_algo(process_list_t *process_list, int quantum, int sch_algo) {
    switch (sch_algo) {
        case SCH_FF:
            fcfs(process_list);
            break;
        case SCH_RR:
            if (quantum == -1) {  // quantum not given
                perror("ERROR trying to run Round-Robin without quantum");
                exit(EXIT_FAILURE);
            }
            rr(process_list, quantum);
            break;
        case BYO:
            sjf(process_list);
            break;
        default:
            perror("ERROR selecting scheduling algorithm");
            exit(EXIT_FAILURE);
    }
}

// first-come first-served
void fcfs(process_list_t *process_list) {

    if (process_list == NULL) {
        perror("ERROR scheduling empty list");
        exit(EXIT_FAILURE);
    }

    process_t *curr_process = process_list->head_process;  // current process
    int time = 0;

    // for stats
    int executed_count = 0;
    // init min with the value of 61 because min job time would be 1 in a 60 unit time frame
    int min_throughput = 61, max_throughput = 0, total_throughput = 0;
    // this keeps track of throughput in each time period
    int throughput = 0, last_timestamp = 0, last_finished = 0;
    int total_turnaround = 0;
    float max_overhead = 0, total_overhead = 0;

    // every loop is one process
    while (curr_process != NULL) {
        // if process arrives
        if (curr_process->arrival_time <= time) {
            // process started running
            run_proc(curr_process, time);
            time += curr_process->job_time;

            // process finished
            finish_proc(curr_process, time, &executed_count, process_list->process_count);
            // curr_process->status = FINISHED;
            // executed_count++;
            // print_status(time, curr_process, process_list->process_count - executed_count);
            
            // calculate stats
            calc_stats(&min_throughput, &max_throughput, &total_throughput, &throughput, &last_timestamp,
                       &total_turnaround, &max_overhead, &total_overhead, &last_finished,
                       time, curr_process);
            
            // move on to the next process
            curr_process = curr_process->next;

        } else {
            time++;  // 1 unit of time pass by if no process arrives
        }

    }

    // print stats
    print_stats(executed_count, total_turnaround, total_overhead, max_overhead, time,
                (int)ceil((float)time / THROUGHPUT_TIMEFRAME), min_throughput, max_throughput);
}

// robin round
void rr(process_list_t *process_list, int quantum) {

    if (process_list == NULL) {
        perror("ERROR scheduling empty list");
        exit(EXIT_FAILURE);
    }

    // init
    int time = 0;

    // for list to arrive
    process_list_t *arrived_list = init_process_list();
    process_t *curr_process, *arriving_process = process_list->head_process;
    int executed_count = 0, arrived_count = 0;

    // for stats
    // init min with the value of 61 because min job time would be 1 in a 60 unit time frame
    int min_throughput = 61, max_throughput = 0, total_throughput = 0;
    // this keeps track of throughput in each time period
    int throughput = 0, last_timestamp = 0, last_finished = 0;
    int total_turnaround = 0;
    float max_overhead = 0, total_overhead = 0;

    // executing all processes
    while (executed_count != process_list->process_count) {

        // let process arrive
        arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);

        // run the first in queue
        curr_process = arrived_list->head_process;
        run_proc(curr_process, time);

        // all process arrived and executing last process
        if ((arrived_count == process_list->process_count) && (curr_process->next == NULL)) {
            time += curr_process->remaining_time;
            // finishing process
            finish_proc(curr_process, time, &executed_count, process_list->process_count);
            
            // calculate stats
            calc_stats(&min_throughput, &max_throughput, &total_throughput, &throughput, &last_timestamp,
                       &total_turnaround, &max_overhead, &total_overhead, &last_finished,
                       time, curr_process);

            // remove finished process
            arrived_list = delete_head_proc(arrived_list);
        } else if (curr_process->remaining_time > quantum) {  
            time += quantum;
            // let process arrive before moving curr proc to the end
            arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);
            curr_process->remaining_time -= quantum;
            // move process to end of queue
            arrived_list = move_proc_to_end(arrived_list, curr_process);
        } else {
            time += curr_process->remaining_time;
            // finishing process
            arrived_list = proc_arrive(arriving_process, arrived_list, time, &arrived_count);
            curr_process->remaining_time = 0;
            finish_proc(curr_process, time, &executed_count, process_list->process_count);
            // curr_process->status = FINISHED;
            // executed_count++;
            // print_status(time, curr_process, process_list->process_count - executed_count);

            // calculate stats
            calc_stats(&min_throughput, &max_throughput, &total_throughput, &throughput, &last_timestamp,
                       &total_turnaround, &max_overhead, &total_overhead, &last_finished,
                       time, curr_process);
            
            // remove finished process
            arrived_list = delete_head_proc(arrived_list);
        }
    }

    // print stats
    print_stats(executed_count, total_turnaround, total_overhead, max_overhead, time,
                (int)ceil((float)time / THROUGHPUT_TIMEFRAME), min_throughput, max_throughput);
}

// shortest job first / shortest process next (spn)
void sjf(process_list_t *process_list) {

    if (process_list == NULL) {
        perror("ERROR scheduling empty list");
        exit(EXIT_FAILURE);
    }
    
    int time = 0;

    // for processes to arrive
    process_list_t *arrived_list = init_process_list();
    process_t *curr_process, *min_process;
    process_t *arriving_process = process_list->head_process;
    
    // for stats
    int executed_count = 0, arrived_count = 0;
    // init min with the value of 61 because min job time would be 1 in a 60 unit time frame
    int min_throughput = 61, max_throughput = 0, total_throughput = 0;
    // this keeps track of throughput in each time period
    int throughput = 0, last_timestamp = 0, last_finished = 0;
    int total_turnaround = 0;
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
            // free(temp);  <- already freed in `delete_head_proc`
        }

        // run the process
        run_proc(min_process, time);
        time += min_process->job_time;

        // finish the process
        finish_proc(min_process, time, &executed_count, process_list->process_count);
        // min_process->status = FINISHED;
        // executed_count++;
        // print_status(time, min_process, process_list->process_count - executed_count);

        // calculate stats
        calc_stats(&min_throughput, &max_throughput, &total_throughput, &throughput, &last_timestamp,
                   &total_turnaround, &max_overhead, &total_overhead, &last_finished,
                   time, min_process);
    }

    // print stats
    print_stats(executed_count, total_turnaround, total_overhead, max_overhead, time,
                (int)ceil((float)time / THROUGHPUT_TIMEFRAME), min_throughput, max_throughput);

}

// calculate required stats
// tp=throughput
void calc_stats(int *min_tp, int *max_tp, int *tot_tp, int *tp, int *last_timestamp,
                int *tot_turnaround, float *max_overhead, float *tot_overhead, int *last_finished,
                int time, process_t *proc) {
    
    // calculate throughput
    if (time < (*last_timestamp) + THROUGHPUT_TIMEFRAME) {
        (*tp)++;
    } else {
        if (time == (*last_timestamp) + THROUGHPUT_TIMEFRAME) {
            (*tp)++;
        }
        // there is an interval where no process was compeleted
        if (time > (*last_finished) + THROUGHPUT_TIMEFRAME) {
            (*min_tp) = 0;
        }
        (*last_timestamp) += THROUGHPUT_TIMEFRAME;
        // update min, max and total
        if ((*tp) > (*max_tp)) (*max_tp) = (*tp);
        if ((*tp) < (*min_tp)) (*min_tp) = (*tp);
        (*tot_tp) += (*tp);
        (*tp) = 1;
    }
    (*last_finished) = time;

    // calculate turnaround
    int turnaround = time - proc->arrival_time ;
    (*tot_turnaround) += turnaround;
    
    // calculate overhead
    float overhead = (float)turnaround / (float)proc->job_time;
    (*tot_overhead) += overhead;
    if (overhead > (*max_overhead)) (*max_overhead) = overhead;

}