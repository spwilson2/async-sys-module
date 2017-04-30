#include "../circle-buffer.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MULTIPLE_PROD_THREADS 5
#define MULTIPLE_CONS_THREADS 5

#define INSERTS 10000

#define QUEUE_SIZE 100

#define FAIL_ERROR { fprintf(stderr, "FAIL\n"); exit(1); }

circle_buffer buffer;

void* insert_num(void* num_insert_p) {
    int num_insert = *(int*)num_insert_p;

    while (--num_insert) {
        push(&buffer, &num_insert);
        //printf("Inserting: %d", num_insert);
    }
    pthread_exit(NULL);
}

void* remove_num(void* num_remove_p) {
    int num_remove = *(int*)num_remove_p;
    int takeout;

    while (--num_remove) {
        pop(&buffer, &takeout);
        //printf("Removing: %d", takeout);
    }
    pthread_exit(NULL);
}

void test_spsc() {
    pthread_t prod_thread;
    pthread_t cons_thread;

    int err;
    int num_insert = INSERTS;

    err = pthread_create(&prod_thread, NULL, insert_num, &num_insert);
    if (err) {
        FAIL_ERROR;
    }

    err = pthread_create(&prod_thread, NULL, remove_num, &num_insert);
    if (err) {
        FAIL_ERROR;
    }
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    printf("SPSC done!\n");
}

void test_mpsc() {
}

void test_spmc() {
}

void test_mpmc() {
}


int main () {
    init_buffer(&buffer, sizeof(int), QUEUE_SIZE);
    test_spsc();
}
