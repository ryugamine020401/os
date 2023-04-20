#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"

#define RAND_DIVISOR 100000000
#define TRUE 1

pthread_mutex_t mutex;
sem_t full, empty;

void *producer(void *param);
void *consumer(void *param);
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void init();

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

/* buffer counter */
int counter;

int main(int argc, char *argv[]) {
    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    int sleepTime = atoi(argv[1]);
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);

    /* 2. Initialize buffer */
    init();

    /* 3. Create producer thread(s) */
    pthread_t producers[numProducers];
    for (int i = 0; i < numProducers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    /* 4. Create consumer thread(s) */
    pthread_t consumers[numConsumers];
    for (int i = 0; i < numConsumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    /* 5. Sleep */
    sleep(sleepTime);

    /* 6. Exit */
    exit(0);
}

/* Producer Thread */
void *producer(void *param) {
    buffer_item item;

    while (TRUE) {
        /* sleep for a random period of time */
        usleep(rand() / RAND_DIVISOR);

        /* generate a random number */
        item = rand();

        /* acquire empty semaphore */
        sem_wait(&empty);

        /* acquire mutex lock */
        pthread_mutex_lock(&mutex);

        /* insert item into buffer */
        if (insert_item(item) == -1) {
            fprintf(stderr, "Producer report error condition\n");
        } else {
            printf("producer produced %d\n", item);
        }

        /* release mutex lock */
        pthread_mutex_unlock(&mutex);

        /* signal full semaphore */
        sem_post(&full);
    }
}

/* Consumer Thread */
void *consumer(void *param) {
    buffer_item item;

    while (TRUE) {
        /* sleep for a random period of time */
        usleep(rand() / RAND_DIVISOR);

        /* acquire full semaphore */
        sem_wait(&full);

        /* acquire mutex lock */
        pthread_mutex_lock(&mutex);

        /* remove item from buffer */
        if (remove_item(&item) == -1) {
            fprintf(stderr, "Consumer report error condition\n");
        } else {
            printf("consumer consumed %d\n", item);
        }

        /* release mutex lock */
        pthread_mutex_unlock(&mutex);

        /* signal empty semaphore */
        sem_post(&empty);
    }
}

/* Insert item into buffer */
int insert_item(buffer_item item) {
    /* When the buffer is full, return -1 indicating an error condition */
    if (counter >= BUFFER_SIZE) {
        return -1;
    }

    /* Insert the item into the buffer */
    buffer[counter] = item;
    counter++;

    return 0;
}

/* Remove an item from the buffer */
int remove_item(buffer_item *item) {
    /* When the buffer is empty, return -1 indicating an error condition */
    if (counter <= 0) {
        return -1;
    }
    /* Remove the item from the buffer and store it in item */
    counter--;
   
    *item = buffer[counter];

    return 0;
    }

/* Initialize the buffer, mutex, and semaphores */
void init() {
/* Initialize buffer counter */
counter = 0;
/* Initialize mutex lock */
pthread_mutex_init(&mutex, NULL);

/* Initialize empty semaphore to BUFFER_SIZE */
sem_init(&empty, 0, BUFFER_SIZE);

/* Initialize full semaphore to 0 */
sem_init(&full, 0, 0);
}