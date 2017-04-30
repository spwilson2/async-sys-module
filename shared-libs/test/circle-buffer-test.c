#include "../circle-buffer.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MULTIPLE_PROD_THREADS 5
#define MULTIPLE_CONS_THREADS 5

#define INSERTS 100000

#define QUEUE_SIZE 100

#define FAIL_ERROR { fprintf(stderr, "FAIL\n"); exit(1); }

circle_buffer buffer;

void* insert_num(void* num_insert_p) {
    int num_insert = *(int*)num_insert_p;

    while (num_insert--) {
        push(&buffer, &num_insert);
        //printf("Inserting: %d", num_insert);
    }
    pthread_exit(NULL);
}

void* remove_num_check_order(void* num_remove_p) {
    int num_remove = *(int*)num_remove_p;
    int takeout;

    while (num_remove--) {
        pop(&buffer, &takeout);
        if (takeout != num_remove) {
            printf("%d, %d\n", num_remove, takeout);
            FAIL_ERROR;
        }
        //printf("Removing: %d", takeout);
    }
    pthread_exit(NULL);
}

void* remove_num(void* num_remove_p) {
    int num_remove = *(int*)num_remove_p;
    int takeout;

    while (num_remove--) {
        pop(&buffer, &takeout);
        //fprintf(stderr, "Removing: %d\n", takeout);
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

    err = pthread_create(&cons_thread, NULL, remove_num_check_order, &num_insert);
    if (err) {
        FAIL_ERROR;
    }
    err = pthread_join(prod_thread, NULL);
    if (err) {
        FAIL_ERROR;
    }
    err = pthread_join(cons_thread, NULL);
    if (err) {
        FAIL_ERROR;
    }
    printf("SPSC done!\n");
}

void test_mpsc() {
    pthread_t prod_thread[MULTIPLE_PROD_THREADS];
    pthread_t cons_thread;

    int err;
    int num_created = (INSERTS/MULTIPLE_PROD_THREADS)*MULTIPLE_PROD_THREADS;
    int num_insert = (INSERTS/MULTIPLE_PROD_THREADS);

    for (int i = 0; i < MULTIPLE_PROD_THREADS; i++) {
        err = pthread_create(&prod_thread[i], NULL, insert_num, &num_insert);
        if (err) {
            FAIL_ERROR;
        }
    }

    err = pthread_create(&cons_thread, NULL, remove_num, &num_created);
    if (err) {
        FAIL_ERROR;
    }

    for (int i = 0; i < MULTIPLE_PROD_THREADS; i++) {
        err = pthread_join(prod_thread[i], NULL);
        if (err) {
            FAIL_ERROR;
        }
    }
    err = pthread_join(cons_thread, NULL);
    if (err) {
        FAIL_ERROR;
    }
    printf("MPSC done!\n");
}

void test_spmc() {
    pthread_t prod_thread;
    pthread_t cons_thread[MULTIPLE_CONS_THREADS];

    int err;
    int num_created = (INSERTS/MULTIPLE_CONS_THREADS)*MULTIPLE_CONS_THREADS;
    int num_remove = (INSERTS/MULTIPLE_CONS_THREADS);

    err = pthread_create(&prod_thread, NULL, insert_num, &num_created);
    if (err) {
        FAIL_ERROR;
    }

    for (int i = 0; i < MULTIPLE_CONS_THREADS; i++) {
        err = pthread_create(&cons_thread[i], NULL, remove_num, &num_remove);
        if (err) {
            FAIL_ERROR;
        }
    }

    err = pthread_join(prod_thread, NULL);
    if (err) {
        FAIL_ERROR;
    }

    for (int i = 0; i < MULTIPLE_CONS_THREADS; i++) {
        err = pthread_join(cons_thread[i], NULL);
        if (err) {
            FAIL_ERROR;
        }
    }
    printf("SPMC done!\n");
}

void test_mpmc() {
    pthread_t prod_thread[MULTIPLE_PROD_THREADS];
    pthread_t cons_thread[MULTIPLE_CONS_THREADS];

    int err;
    int num_insert = (INSERTS/MULTIPLE_PROD_THREADS);
    int num_remove = (INSERTS/MULTIPLE_CONS_THREADS);

    for (int i = 0; i < MULTIPLE_PROD_THREADS; i++) {
        err = pthread_create(&prod_thread[i], NULL, insert_num, &num_insert);
        if (err) {
            FAIL_ERROR;
        }
    }
    for (int i = 0; i < MULTIPLE_CONS_THREADS; i++) {
        err = pthread_create(&cons_thread[i], NULL, remove_num, &num_remove);
        if (err) {
            FAIL_ERROR;
        }
    }

    for (int i = 0; i < MULTIPLE_PROD_THREADS; i++) {
        err = pthread_join(prod_thread[i], NULL);
        if (err) {
            FAIL_ERROR;
        }
    }

    for (int i = 0; i < MULTIPLE_CONS_THREADS; i++) {
        err = pthread_join(cons_thread[i], NULL);
        if (err) {
            FAIL_ERROR;
        }
    }

    printf("MPMC done!\n");
}


int main () {
    init_buffer(&buffer, sizeof(int), QUEUE_SIZE);
    test_spsc();
    test_mpsc();
    test_spmc();
    test_mpmc();
    return 0;
}
