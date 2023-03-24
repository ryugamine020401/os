#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 12

/* Global variables */
int arr[SIZE] = {39, 10 ,9, 4, 6, 3, 7, 1, 2, 5, 8, 0};
int sorted_arr[SIZE];
int mid = SIZE / 2;

/* Structure for passing data to threads */
typedef struct {
    int starting;
    int ending;
} parameters;

/* Function prototypes */
void *sorting_thread(void *params);
void *merging_thread();

int main() {
    int i;
    pthread_t threads[3];
    pthread_attr_t attr[3];
    parameters *data;

    /* Create two sorting threads */
    for (i = 0; i < 2; i++) {
        data = (parameters *) malloc(sizeof(parameters));
        data->starting = i * mid;
        data->ending = (i + 1) * mid - 1;
        pthread_attr_init(&attr[i]);
        pthread_create(&threads[i], &attr[i], sorting_thread, data);
    }

    /* Wait for sorting threads to complete */
    for (i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Create merging thread */
    pthread_attr_init(&attr[2]);
    pthread_create(&threads[2], &attr[2], merging_thread, NULL);

    /* Wait for merging thread to complete */
    pthread_join(threads[2], NULL);

    /* Output sorted array */
    printf("Sorted array: ");
    for (i = 0; i < SIZE; i++) {
        printf("%d ", sorted_arr[i]);
    }
    printf("\n");

    return 0;
}

/* Sorting thread function */
void *sorting_thread(void *params) {
    int i, j, temp;
    parameters *data = (parameters *) params;
    int start = data->starting;
    int end = data->ending;
    // for(int c=0; c<SIZE; c++){
    //     printf("%d ", arr[c]);
    // }
    // printf(" state=%d,end=%d\n",start,end);
    /* Bubble sort */
    for (i = start; i < end; i++) {
        for (j = start; j < start+end-i; j++) {
            if (arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    // for(int c=0; c<SIZE; c++){
    //     printf("%d ", arr[c]);
    // }
    // printf("state=%d,end=%d\n",start,end);
    

    pthread_exit(NULL);
}

/* Merging thread function */
void *merging_thread() {
    int i = 0, j = mid, k = 0;
    for(int c = 0;c<SIZE;c++){
        printf("%d ", arr[c]);
    }
    printf("\n");
    /* Merge two sorted sublists */
    while (i < mid && j < SIZE) {
        if (arr[i] < arr[j]) {                  
            sorted_arr[k++] = arr[i++];         /*i 3 4 6 9 10 39*/      
        } else {                                /*j 0 1 2 5 07 08*/
            sorted_arr[k++] = arr[j++];         /*k 0 1 2 3 4 5 6 7 8*/
        }
    }
    for(int c = 0;c<SIZE;c++){
        printf("%d ", sorted_arr[c]);
    }
    printf("\n");

    /* Append remaining elements from first sublist */
    while (i < mid) {
        sorted_arr[k++] = arr[i++];
    }

    for(int c = 0;c<SIZE;c++){
        printf("%d ", sorted_arr[c]);
    }
    printf("\n");

    /* Append remaining elements from second sublist */
    while (j < SIZE) {
        sorted_arr[k++] = arr[j++];
    }

    pthread_exit(NULL);
}

