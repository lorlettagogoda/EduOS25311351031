// thread_manager.c

#include "include/eduos.h"

#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>

#define THREAD_POOL_SIZE 4
#define TASK_QUEUE_SIZE 20

// ======================================================
// TASK STRUCTURE
// ======================================================

typedef struct {

    int task_id;

    void (*task_function)(void *);

    void *arg;

} Task;

// ======================================================
// THREAD POOL VARIABLES
// ======================================================

pthread_t thread_pool[THREAD_POOL_SIZE];

Task task_queue[TASK_QUEUE_SIZE];

int task_front = 0;
int task_rear = 0;
int task_count = 0;

// ======================================================
// SYNCHRONIZATION
// ======================================================

pthread_mutex_t queue_mutex;

pthread_cond_t queue_cond;

int shutdown_pool = 0;

// ======================================================
// RACE CONDITION VARIABLES
// ======================================================

int shared_counter = 0;

pthread_mutex_t counter_mutex;

// ======================================================
// SEMAPHORE VARIABLES
// ======================================================

sem_t empty;
sem_t full;
sem_t buffer_mutex;

int buffer[5];
int buffer_index = 0;

// ======================================================
// DEADLOCK VARIABLES
// ======================================================

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

// ======================================================
// MANY TO ONE VARIABLES
// ======================================================

jmp_buf environment;

// ======================================================
// SAMPLE TASK
// ======================================================

void sample_task(void *arg) {

    int num = *((int *)arg);

    printf("Thread executing task %d\n", num);

    sleep(1);

    printf("Task %d completed\n", num);
}

// ======================================================
// WORKER THREAD FUNCTION
// ======================================================

void* worker_thread(void *arg) {

    while(1) {

        pthread_mutex_lock(&queue_mutex);

        while(task_count == 0 && !shutdown_pool) {

            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        if(shutdown_pool) {

            pthread_mutex_unlock(&queue_mutex);

            pthread_exit(NULL);
        }

        Task task = task_queue[task_front];

        task_front = (task_front + 1) % TASK_QUEUE_SIZE;

        task_count--;

        pthread_mutex_unlock(&queue_mutex);

        task.task_function(task.arg);
    }

    return NULL;
}

// ======================================================
// CREATE THREAD POOL
// ======================================================

void create_thread_pool() {

    pthread_mutex_init(&queue_mutex, NULL);

    pthread_cond_init(&queue_cond, NULL);

    for(int i = 0; i < THREAD_POOL_SIZE; i++) {

        pthread_create(&thread_pool[i],
                       NULL,
                       worker_thread,
                       NULL);

        printf("Created worker thread %d\n", i);
    }
}

// ======================================================
// ADD TASK TO QUEUE
// ======================================================

void add_task(void (*function)(void *), void *arg, int task_id) {

    pthread_mutex_lock(&queue_mutex);

    Task task;

    task.task_id = task_id;

    task.task_function = function;

    task.arg = arg;

    task_queue[task_rear] = task;

    task_rear = (task_rear + 1) % TASK_QUEUE_SIZE;

    task_count++;

    printf("Added task %d to queue\n", task_id);

    pthread_cond_signal(&queue_cond);

    pthread_mutex_unlock(&queue_mutex);
}

// ======================================================
// DESTROY THREAD POOL
// ======================================================

void destroy_thread_pool() {

    pthread_mutex_lock(&queue_mutex);

    shutdown_pool = 1;

    pthread_cond_broadcast(&queue_cond);

    pthread_mutex_unlock(&queue_mutex);

    for(int i = 0; i < THREAD_POOL_SIZE; i++) {

        pthread_join(thread_pool[i], NULL);

        printf("Thread %d terminated\n", i);
    }

    pthread_mutex_destroy(&queue_mutex);

    pthread_cond_destroy(&queue_cond);
}

// ======================================================
// RACE CONDITION DEMO
// ======================================================

void* race_condition(void *arg) {

    for(int i = 0; i < 100000; i++) {

        shared_counter++;
    }

    return NULL;
}

// ======================================================
// FIXED RACE CONDITION
// ======================================================

void* fixed_race_condition(void *arg) {

    for(int i = 0; i < 100000; i++) {

        pthread_mutex_lock(&counter_mutex);

        shared_counter++;

        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

// ======================================================
// PRODUCER
// ======================================================

void* producer(void *arg) {

    for(int i = 0; i < 5; i++) {

        sem_wait(&empty);

        sem_wait(&buffer_mutex);

        buffer[buffer_index++] = i;

        printf("Produced %d\n", i);

        sem_post(&buffer_mutex);

        sem_post(&full);

        sleep(1);
    }

    return NULL;
}

// ======================================================
// CONSUMER
// ======================================================

void* consumer(void *arg) {

    for(int i = 0; i < 5; i++) {

        sem_wait(&full);

        sem_wait(&buffer_mutex);

        int item = buffer[--buffer_index];

        printf("Consumed %d\n", item);

        sem_post(&buffer_mutex);

        sem_post(&empty);

        sleep(1);
    }

    return NULL;
}

// ======================================================
// DEADLOCK DEMONSTRATION
// ======================================================

void* thread_a(void *arg) {

    pthread_mutex_lock(&mutex1);

    printf("Thread A locked mutex1\n");

    sleep(1);

    pthread_mutex_lock(&mutex2);

    printf("Thread A locked mutex2\n");

    pthread_mutex_unlock(&mutex2);

    pthread_mutex_unlock(&mutex1);

    return NULL;
}

void* thread_b(void *arg) {

    pthread_mutex_lock(&mutex2);

    printf("Thread B locked mutex2\n");

    sleep(1);

    pthread_mutex_lock(&mutex1);

    printf("Thread B locked mutex1\n");

    pthread_mutex_unlock(&mutex1);

    pthread_mutex_unlock(&mutex2);

    return NULL;
}

// ======================================================
// FIXED DEADLOCK VERSION
// ======================================================

void* safe_thread(void *arg) {

    pthread_mutex_lock(&mutex1);

    pthread_mutex_lock(&mutex2);

    printf("Safe thread acquired both mutexes\n");

    pthread_mutex_unlock(&mutex2);

    pthread_mutex_unlock(&mutex1);

    return NULL;
}

// ======================================================
// MANY TO ONE THREADING
// ======================================================

void many_to_one_task1() {

    printf("User thread 1 running\n");

    longjmp(environment, 1);
}

void many_to_one_task2() {

    printf("User thread 2 running\n");
}

// ======================================================
// ONE TO ONE THREADING
// ======================================================

void* one_to_one_task(void *arg) {

    int num = *((int*)arg);

    long sum = 0;

    for(int i = 0; i < 1000000; i++) {

        sum += i;
    }

    printf("POSIX thread %d completed sum = %ld\n",
           num,
           sum);

    return NULL;
}

// ======================================================
// THREAD POOL DEMO
// ======================================================

void run_thread_pool_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("THREAD POOL DEMO\n");
    printf("=====================================\n");

    create_thread_pool();

    int nums[5];

    for(int i = 0; i < 5; i++) {

        nums[i] = i + 1;

        add_task(sample_task, &nums[i], i + 1);
    }

    sleep(5);

    destroy_thread_pool();
}

// ======================================================
// RACE CONDITION DEMO
// ======================================================

void run_race_condition_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("RACE CONDITION DEMO\n");
    printf("=====================================\n");

    pthread_t t1, t2;

    shared_counter = 0;

    pthread_create(&t1, NULL, race_condition, NULL);

    pthread_create(&t2, NULL, race_condition, NULL);

    pthread_join(t1, NULL);

    pthread_join(t2, NULL);

    printf("Counter without mutex = %d\n", shared_counter);
}

// ======================================================
// FIXED RACE CONDITION DEMO
// ======================================================

void run_fixed_race_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("FIXED RACE CONDITION\n");
    printf("=====================================\n");

    pthread_t t1, t2;

    pthread_mutex_init(&counter_mutex, NULL);

    shared_counter = 0;

    pthread_create(&t1, NULL, fixed_race_condition, NULL);

    pthread_create(&t2, NULL, fixed_race_condition, NULL);

    pthread_join(t1, NULL);

    pthread_join(t2, NULL);

    printf("Counter with mutex = %d\n", shared_counter);

    pthread_mutex_destroy(&counter_mutex);
}

// ======================================================
// PRODUCER CONSUMER DEMO
// ======================================================

void run_producer_consumer_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("PRODUCER CONSUMER DEMO\n");
    printf("=====================================\n");

    pthread_t prod, cons;

    sem_init(&empty, 0, 5);

    sem_init(&full, 0, 0);

    sem_init(&buffer_mutex, 0, 1);

    pthread_create(&prod, NULL, producer, NULL);

    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);

    pthread_join(cons, NULL);

    sem_destroy(&empty);

    sem_destroy(&full);

    sem_destroy(&buffer_mutex);
}

// ======================================================
// DEADLOCK DEMO
// ======================================================

void run_deadlock_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("DEADLOCK FIX DEMO\n");
    printf("=====================================\n");

    pthread_mutex_init(&mutex1, NULL);

    pthread_mutex_init(&mutex2, NULL);

    pthread_t s1, s2;

    pthread_create(&s1, NULL, safe_thread, NULL);

    pthread_create(&s2, NULL, safe_thread, NULL);

    pthread_join(s1, NULL);

    pthread_join(s2, NULL);

    pthread_mutex_destroy(&mutex1);

    pthread_mutex_destroy(&mutex2);
}

// ======================================================
// MANY TO ONE DEMO
// ======================================================

void run_many_to_one_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("MANY TO ONE MODEL\n");
    printf("=====================================\n");

    if(setjmp(environment) == 0) {

        many_to_one_task1();

    } else {

        many_to_one_task2();
    }
}

// ======================================================
// ONE TO ONE DEMO
// ======================================================

void run_one_to_one_demo() {

    printf("\n");
    printf("=====================================\n");
    printf("ONE TO ONE MODEL\n");
    printf("=====================================\n");

    pthread_t workers[2];

    int ids[2] = {1, 2};

    pthread_create(&workers[0],
                   NULL,
                   one_to_one_task,
                   &ids[0]);

    pthread_create(&workers[1],
                   NULL,
                   one_to_one_task,
                   &ids[1]);

    pthread_join(workers[0], NULL);

    pthread_join(workers[1], NULL);

    printf("\nOne-to-One threading completed.\n");
}