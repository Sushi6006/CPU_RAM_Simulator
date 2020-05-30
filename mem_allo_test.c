#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "scheduler.h"
#include "output.h"
#include "processlist.h"
#include "memory.h"

// testing `evict_proc()` -- PASS
void test_evict() {
    unit_t *mem_list = init_memory_list(10);
    mem_list[0].proc_id = HOLE;
    mem_list[1].proc_id = HOLE;
    mem_list[2].proc_id = HOLE;
    mem_list[3].proc_id = 1;
    mem_list[4].proc_id = 1;
    mem_list[5].proc_id = 1;
    mem_list[6].proc_id = 1;
    mem_list[7].proc_id = HOLE;
    mem_list[8].proc_id = 2;
    mem_list[9].proc_id = 2;

    evict_proc(mem_list, 10, 1);
    print_memory_list(mem_list, 10);
}

// testing `update_status()`  -- PASS
void test_update_status() {
    unit_t *mem_list = init_memory_list(10);
    mem_list[0].proc_id = HOLE;
    mem_list[1].proc_id = HOLE;
    mem_list[2].proc_id = HOLE;
    mem_list[3].proc_id = 1;
    mem_list[4].proc_id = 1;
    mem_list[5].proc_id = 1;
    mem_list[6].proc_id = 1;
    mem_list[7].proc_id = HOLE;
    mem_list[8].proc_id = 2;
    mem_list[9].proc_id = 2;
    print_memory_list(mem_list, 10);
    print_status_list(update_status(mem_list, 10));
    
}

int main() {

    // test_update_status();
    int size = 20, time = 0;
    process_list_t *proc_list = init_process_list();
    unit_t *mem_list = init_memory_list(size);
    status_list_t *status_list = init_status_list(size);
    
    // add some proc
    proc_list = add_process(proc_list, create_process(0, 1, 15, 20));
    proc_list = add_process(proc_list, create_process(0, 2, 5, 20));
    proc_list = add_process(proc_list, create_process(0, 3, 10, 20));

    // run procs
    process_t *curr_proc = proc_list->head_process;
    int counter = 0;
    while (curr_proc != NULL) {
        printf("======= %d =======\n", counter);
        run_proc(curr_proc, time);
        status_list = allocate_proc(mem_list, size, status_list, curr_proc, time);
        print_memory_list(mem_list, size);
        // print_status_list(status_list);
        time += curr_proc->job_time;
        curr_proc = curr_proc->next;
        counter++;
    }
    return 0;
}