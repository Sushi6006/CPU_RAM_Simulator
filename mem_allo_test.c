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

    // evict_proc(mem_list, 10, 1,);  // modified later, not working anymore
    // print_memory_list(mem_list, 10);
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

int run_lah(process_list_t *proc_list, unit_t *mem_list, status_list_t *status_list, spec_t spec) {

    int time = 0;
    process_t *proc = proc_list->head_process;
    while (proc != NULL) {
        time += run_proc(proc, time, mem_list, spec);
        time += proc->job_time;
        int count = 0;
        finish_proc(proc, time, mem_list, spec, &count);
        proc = proc->next;
    }

    print_memory_list(mem_list, spec.mem_size);
    printf("TIME:  %d\n", time);

    return time;
}

int main() {

    // test_update_status();
    int size = 20;
    process_list_t *proc_list = init_process_list();
    unit_t *mem_list = init_memory_list(size);
    status_list_t *status_list = init_status_list(size);
    
    // add some proc
    proc_list = add_process(proc_list, create_process(0, 1, 5, 20));
    proc_list = add_process(proc_list, create_process(0, 2, 5, 20));
    proc_list = add_process(proc_list, create_process(0, 3, 5, 20));
    proc_list = add_process(proc_list, create_process(0, 4, 5, 20));
    proc_list = add_process(proc_list, create_process(0, 5, 15, 20));

    // spec
    spec_t spec;
    spec.mem_allo = 'p';
    spec.mem_size = 20;
    spec.proc_count = 5;
    spec.quantum = -1;
    spec.sch_algo = 'f';

    run_lah(proc_list, mem_list, status_list, spec);

    return 0;
}