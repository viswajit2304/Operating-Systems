#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 100000 

int buffer[MAX_BUFFER_SIZE];
int bufferSize = 0;
int in = 0, out = 0;
int P,C,S,N;
int flag;

pthread_mutex_t mutex ;
pthread_cond_t full ;
pthread_cond_t empty ;
void initializeSyncObjects() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
}

void destroySyncObjects() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
}

// Function for the producer
void *producer(void *arg) {
    int thread_number = *((int *)arg);
    int i=0;
    do{
        int item = (rand() % 9000) + 1000; 
        pthread_mutex_lock(&mutex);
        while (bufferSize == S) {
            if(flag == 0)
            {
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&full, &mutex);
        }
        buffer[in] = item;
        printf("Producer Thread #: %d; Item #: %d; Added Number: %d\n", thread_number, i+1, item);
        in = (in + 1) % S;
        bufferSize++;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        
        usleep(rand() % 3000000); // Sleep for 0-3 seconds
        i++;
    }while(i<N);
    pthread_exit(NULL);
}


void *consumer(void *arg) {
    int thread_number = *((int *)arg);
    
    int items_to_consume = 0.7 *(N*P)/C;
    while (items_to_consume > 0) {
        pthread_mutex_lock(&mutex);
        while (bufferSize == 0) {
            pthread_cond_wait(&empty, &mutex);
        }
        int item = buffer[out];
        printf("Consumer Thread #: %d; Item #: %d; Read Number: %d\n", thread_number, items_to_consume, item);
        out = (out + 1) % S;
        bufferSize--;
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
        flag--;
        if(flag==0) pthread_cond_broadcast(&full);
        
        usleep(rand() % 4000000); // Sleep for 0-4 seconds
        items_to_consume--;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    initializeSyncObjects();
    // Assuming argv[1] is "-p", argv[2] is the value for -p, and so on
    C = atoi(argv[4]);
    P = atoi(argv[2]);
    N = atoi(argv[8]);
    S = atoi(argv[6]);

    int limit = (int)(0.7*N*P/C);
    pthread_t producer_threads[P];
    flag = limit*C;
    // Create producer and consumer threads
    pthread_t consumer_threads[C];
    
    // Initialize producer threads
    int i=0;
    while(i<P)
    {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&producer_threads[i], NULL, producer, arg);   
        i++;
    }
    i=0;
    // Initialize consumer threads
    while(i<C){
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&consumer_threads[i], NULL, consumer, arg);
        i++;
    }
    i=0;
    while(i<P){
        pthread_join(producer_threads[i], NULL);
        i++;
    }
    i=0;
    while(i<C){
        pthread_join(consumer_threads[i], NULL);
        i++;
    }
    printf("Program over...\n");
    destroySyncObjects();
    return 0;
}