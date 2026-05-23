// ipc_module.c

#include "include/eduos.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

// ======================================================
// SHARED MEMORY STRUCT
// ======================================================

typedef struct {

    int owner_id;

    int total_processes;

    int cpu_usage;

    char message[100];

} SharedData;

// ======================================================
// SHARED MEMORY DEMO
// ======================================================

void run_shared_memory_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("SHARED MEMORY DEMO\n");
    printf("=====================================\n");

    const char *shared_memory_name = "/eduos_shared_memory";

    int shm_fd;

    SharedData *shared_data;

    // --------------------------------------------------
    // CREATE SHARED MEMORY
    // --------------------------------------------------

    shm_fd = shm_open(shared_memory_name,
                      O_CREAT | O_RDWR,
                      0666);

    if(shm_fd < 0) {

        perror("shm_open failed");

        return;
    }

    // allocate memory size
    if(ftruncate(shm_fd, sizeof(SharedData)) == -1) {

        perror("ftruncate failed");

        return;
    }

    // map memory
    shared_data = mmap(NULL,
                       sizeof(SharedData),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       shm_fd,
                       0);

    if(shared_data == MAP_FAILED) {

        perror("mmap failed");

        return;
    }

    // --------------------------------------------------
    // PARENT PROCESS WRITES DATA
    // --------------------------------------------------

    shared_data->owner_id = 1001;

    shared_data->total_processes = 5;

    shared_data->cpu_usage = 78;

    strcpy(shared_data->message,
           "Shared memory communication successful");

    printf("Parent wrote to shared memory.\n");

    // --------------------------------------------------
    // CREATE CHILD PROCESS
    // --------------------------------------------------

    pid_t pid = fork();

    if(pid < 0) {

        perror("fork failed");

        return;
    }

    // --------------------------------------------------
    // CHILD PROCESS
    // --------------------------------------------------

    if(pid == 0) {

        printf("\nChild process attempting access...\n");

        int child_owner_id = 1001;

        // ACCESS CONTROL CHECK
        if(child_owner_id == shared_data->owner_id) {

            printf("Access granted.\n");

            printf("Total Processes: %d\n",
                   shared_data->total_processes);

            printf("CPU Usage: %d%%\n",
                   shared_data->cpu_usage);

            printf("Message: %s\n",
                   shared_data->message);

        } else {

            printf("Access denied.\n");
        }

        exit(0);
    }

    // --------------------------------------------------
    // PARENT WAITS
    // --------------------------------------------------

    wait(NULL);

    // --------------------------------------------------
    // CLEANUP
    // --------------------------------------------------

    munmap(shared_data, sizeof(SharedData));

    close(shm_fd);

    shm_unlink(shared_memory_name);

    printf("Shared memory cleaned up.\n");
}

// ======================================================
// PIPE DEMO
// ======================================================

void run_pipe_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("PIPE IPC DEMO\n");
    printf("=====================================\n");

    int fd[2];

    if(pipe(fd) == -1) {

        perror("pipe failed");

        return;
    }

    pid_t pid = fork();

    if(pid < 0) {

        perror("fork failed");

        return;
    }

    // --------------------------------------------------
    // CHILD PROCESS
    // --------------------------------------------------

    if(pid == 0) {

        close(fd[1]);

        char buffer[256];

        read(fd[0], buffer, sizeof(buffer));

        printf("\nChild received PCB data:\n");

        printf("%s\n", buffer);

        close(fd[0]);

        exit(0);
    }

    // --------------------------------------------------
    // PARENT PROCESS
    // --------------------------------------------------

    else {

        close(fd[0]);

        char pcb_data[256];

        sprintf(pcb_data,
                "{ PID: %d, NAME: %s, STATE: %s }",

                pcb_table[0].pid,

                pcb_table[0].name,

                state_to_string(pcb_table[0].state)
        );

        write(fd[1],
              pcb_data,
              strlen(pcb_data) + 1);

        printf("Parent sent PCB data through pipe.\n");

        close(fd[1]);

        wait(NULL);
    }

    printf("Pipe communication completed.\n");
}