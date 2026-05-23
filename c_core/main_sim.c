// main_sim.c

#include "include/eduos.h"

// ======================================================
// MAIN SIMULATOR
// ======================================================

int main() {

    printf("\n");
    printf("=====================================\n");
    printf("         EDUOS SIMULATOR START\n");
    printf("=====================================\n");

    // ==================================================
    // CREATE INITIAL PROCESS
    // ==================================================

    PCB init_process;

    init_process.pid = 1;

    strcpy(init_process.name, "init");

    init_process.state = RUNNING;

    init_process.priority = 1;

    init_process.burst_time = 20;

    init_process.remaining_time = 20;

    init_process.arrival_time = 0;

    init_process.memory_req_kb = 512;

    init_process.thread_count = 1;

    init_process.creation_time = time(NULL);

    init_process.parent_pid = 0;

    init_process.exit_code = 0;

    init_process.owner_id = 1001;

    // Add initial process to PCB table
    pcb_table[pcb_count++] = init_process;

    // Save snapshot to JSON
    save_pcb_snapshot();

    // ==================================================
    // PROCESS MANAGEMENT DEMO
    // ==================================================

    printf("\n");
    printf("=====================================\n");
    printf("PROCESS MANAGEMENT DEMO\n");
    printf("=====================================\n");

    // Create child process
    pid_t child_pid = edu_fork(&init_process);

    // Execute new program
    edu_exec(child_pid, "chrome.exe");

    // Show process table
    edu_ps();

    // Terminate child
    edu_exit(child_pid, 0);

    // Parent waits for child
    edu_wait(init_process.pid);

    // Show updated table
    edu_ps();

    // ==================================================
    // THREADING DEMONSTRATIONS
    // ==================================================

    printf("\n");
    printf("=====================================\n");
    printf("THREADING DEMOS\n");
    printf("=====================================\n");

    run_thread_pool_demo();

    run_race_condition_demo();

    run_fixed_race_demo();

    run_producer_consumer_demo();

    run_deadlock_demo();

    run_many_to_one_demo();

    run_one_to_one_demo();

    // ==================================================
    // IPC DEMONSTRATIONS
    // ==================================================

    printf("\n");
    printf("=====================================\n");
    printf("IPC DEMOS\n");
    printf("=====================================\n");

    run_shared_memory_demo();

    run_pipe_demo();

    // ==================================================
    // FINAL PROCESS TABLE
    // ==================================================

    printf("\n");
    printf("=====================================\n");
    printf("FINAL PROCESS TABLE\n");
    printf("=====================================\n");

    edu_ps();

    // Save final PCB snapshot
    save_pcb_snapshot();

    // ==================================================
    // END SIMULATION
    // ==================================================

    printf("\n");
    printf("=====================================\n");
    printf("     EDUOS SIMULATION COMPLETE\n");
    printf("=====================================\n");

    return 0;
}