#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty, full;

int insert_item(buffer_item item) {
    // wait for empty slots
    sem_wait(&empty);

    // acquire mutex lock
    pthread_mutex_lock(&mutex);

    // insert item into buffer
    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {
        if (buffer[i] == -1) {
            buffer[i] = item;
            break;
        }
    }

    // release mutex lock
    pthread_mutex_unlock(&mutex);

    // signal full slots
    sem_post(&full);

    return 0;
}

int remove_item(buffer_item *item) {
    // wait for full slots
    sem_wait(&full);

    // acquire mutex lock
    pthread_mutex_lock(&mutex);

    // remove item from buffer
    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {
        if (buffer[i] != -1) {
            *item = buffer[i];
            buffer[i] = -1;
            break;
        }
    }

    // release mutex lock
    pthread_mutex_unlock(&mutex);

    // signal empty slots
    sem_post(&empty);

    return 0;
}

void initialize_buffer() {
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = -1;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
}