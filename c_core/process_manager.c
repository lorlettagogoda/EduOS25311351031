#include "include/eduos.h"

// ---------------- GLOBAL VARIABLES ----------------
PCB pcb_table[MAX_PROCESSES];

int pcb_count = 0;

pid_t next_pid = 1;

// ---------------- PRINT TIMESTAMP ----------------
void print_timestamp() {

    time_t now = time(NULL);

    struct tm *t = localtime(&now);

    printf("[%02d:%02d:%02d] ",
           t->tm_hour,
           t->tm_min,
           t->tm_sec);
}

// ---------------- CONVERT STATE TO STRING ----------------
const char* state_to_string(int state) {

    switch(state) {

        case NEW:
            return "NEW";

        case READY:
            return "READY";

        case RUNNING:
            return "RUNNING";

        case WAITING:
            return "WAITING";

        case TERMINATED:
            return "TERMINATED";

        default:
            return "UNKNOWN";
    }
}

// ---------------- SAVE PCB SNAPSHOT TO JSON ----------------
void save_pcb_snapshot() {

    FILE *fp = fopen("pcb_snapshot.json", "w");

    if(fp == NULL) {

        perror("Error opening pcb_snapshot.json");

        return;
    }

    fprintf(fp, "[\n");

    for(int i = 0; i < pcb_count; i++) {

        fprintf(fp,
                "  {\n"
                "    \"pid\": %d,\n"
                "    \"name\": \"%s\",\n"
                "    \"state\": \"%s\",\n"
                "    \"priority\": %d,\n"
                "    \"burst_time\": %d,\n"
                "    \"arrival_time\": %d,\n"
                "    \"remaining_time\": %d,\n"
                "    \"memory_req_kb\": %d,\n"
                "    \"thread_count\": %d,\n"
                "    \"parent_pid\": %d,\n"
                "    \"exit_code\": %d\n"
                "  }",

                pcb_table[i].pid,
                pcb_table[i].name,
                state_to_string(pcb_table[i].state),
                pcb_table[i].priority,
                pcb_table[i].burst_time,
                pcb_table[i].arrival_time,
                pcb_table[i].remaining_time,
                pcb_table[i].memory_req_kb,
                pcb_table[i].thread_count,
                pcb_table[i].parent_pid,
                pcb_table[i].exit_code
        );

        if(i < pcb_count - 1) {

            fprintf(fp, ",");
        }

        fprintf(fp, "\n");
    }

    fprintf(fp, "]\n");

    fclose(fp);
}

// ---------------- CREATE CHILD PROCESS ----------------
pid_t edu_fork(PCB *parent) {

    if(pcb_count >= MAX_PROCESSES) {

        printf("Process table full!\n");

        return -1;
    }

    PCB child = *parent;

    child.pid = next_pid++;

    child.parent_pid = parent->pid;

    child.state = NEW;

    child.creation_time = time(NULL);

    pcb_table[pcb_count] = child;

    pcb_count++;

    print_timestamp();

    printf("Forked child process PID %d from parent PID %d\n",
           child.pid,
           parent->pid);

    // Change NEW → READY
    pcb_table[pcb_count - 1].state = READY;

    save_pcb_snapshot();

    return child.pid;
}

// ---------------- EXEC PROCESS ----------------
void edu_exec(pid_t pid, char *prog_name) {

    for(int i = 0; i < pcb_count; i++) {

        if(pcb_table[i].pid == pid) {

            strcpy(pcb_table[i].name, prog_name);

            pcb_table[i].burst_time = 10;

            pcb_table[i].remaining_time = 10;

            print_timestamp();

            printf("Process PID %d executed program %s\n",
                   pid,
                   prog_name);

            save_pcb_snapshot();

            return;
        }
    }

    printf("Process PID %d not found!\n", pid);
}

// ---------------- WAIT FOR CHILD ----------------
int edu_wait(pid_t parent_pid) {

    int found_child = 0;

    for(int i = 0; i < pcb_count; i++) {

        if(pcb_table[i].parent_pid == parent_pid) {

            found_child = 1;

            while(pcb_table[i].state != TERMINATED) {

                print_timestamp();

                printf("Parent PID %d waiting for child PID %d...\n",
                       parent_pid,
                       pcb_table[i].pid);

                sleep(1);
            }

            print_timestamp();

            printf("Child PID %d finished execution\n",
                   pcb_table[i].pid);

            return pcb_table[i].exit_code;
        }
    }

    if(!found_child) {

        printf("No child process found.\n");
    }

    return -1;
}

// ---------------- TERMINATE PROCESS ----------------
void edu_exit(pid_t pid, int exit_code) {

    for(int i = 0; i < pcb_count; i++) {

        if(pcb_table[i].pid == pid) {

            pcb_table[i].state = TERMINATED;

            pcb_table[i].exit_code = exit_code;

            print_timestamp();

            printf("Process PID %d terminated with exit code %d\n",
                   pid,
                   exit_code);

            save_pcb_snapshot();

            return;
        }
    }

    printf("Process PID %d not found!\n", pid);
}

// ---------------- PRINT PROCESS TABLE ----------------
void edu_ps() {

    printf("\n");

    printf("=========================================================\n");

    printf("PID\tNAME\t\tSTATE\t\tPRIORITY\n");

    printf("=========================================================\n");

    for(int i = 0; i < pcb_count; i++) {

        printf("%d\t%-15s %-15s %d\n",

               pcb_table[i].pid,

               pcb_table[i].name,

               state_to_string(pcb_table[i].state),

               pcb_table[i].priority);
    }

    printf("=========================================================\n");
}
