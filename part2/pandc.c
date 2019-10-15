/*
Student Name : Nayan Pandey
Student ID : 920065390
Date  : 8/10/2019
Class : CSC415-02
*/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

//global variables
int *buffer;
int bufferIndex;
int numOfElements;
int itemsProduced;
int itemsConsumed;
int Ptime;
int Ctime;
int global_counter;
int *producerArray;
int *consumerArray;
int producerOfIndex;
int consumerOfIndex;
pthread_mutex_t lock;
sem_t full;
sem_t empty;

//functions to be implemented
void enqueue(int *item);
void dequeue(int *item);
void show_time();
int compare( const void* a, const void* b);
int check_match(int sizeOfProducer, int sizeOfConsumer);
void print_and_check_match_array(int sizeOfProducer, int sizeOfConsumer);

int main(int argc, char *argv[]) {

    //count and print show_time
    show_time();
    time_t begin = time(NULL);
    printf("\n\n\n");
    //parse command line args
    if(argc == 7) {
        int numofProducer, numofConsumer;

        //inititalize all the variables
        bufferIndex = 0;
        numOfElements = atoi(argv[1]);
        numofProducer = atoi(argv[2]);
        numofConsumer = atoi(argv[3]);
        itemsProduced = atoi(argv[4]);
        itemsConsumed = (numofProducer * itemsProduced / numofConsumer);
        Ptime = atoi(argv[5]);
        Ctime = atoi(argv[6]);
        buffer = (int*)malloc(sizeof(int) * numOfElements);

        //print the command line arguments
        printf("                        Number of Buffers: %3s\n", argv[1]);
        printf("                      Number of Producers: %3s\n", argv[2]);
        printf("                      Number of Consumers: %3s\n", argv[3]);
        printf("Number of items Produced by each producer: %3s\n", argv[4]);
        printf("Number of items Consumed by each consumer: %3d\n",itemsConsumed);
        printf("                         Over consume on?:   1\n");
        printf("                      Over consume amount:   6\n");
        printf("     Time each Producer Sleeps (seconds) : %3s\n", argv[5]);
        printf("     Time each Consumer Sleeps (seconds) : %3s\n", argv[6]);
        
        producerArray = (int*)malloc(sizeof(int) * (numofProducer * itemsProduced));
        consumerArray = (int*)malloc(sizeof(int) * (numofConsumer * itemsConsumed));

        //initialize the semaphores and mutex
        //full = dispatch_semaphore_create(0);
        //empty = dispatch_semaphore_create(numOfElements);
        sem_init(&full, 0, 0);
        sem_init(&empty, 0, numOfElements);
        if(pthread_mutex_init(&lock, NULL) != 0) {
            printf("mutex init failed\n");
            return 1;
        }
        global_counter=1;
        //initialize the threads
        pthread_t *tid_1 = (pthread_t*)malloc(sizeof(pthread_t) * numofProducer);
        pthread_t *tid_2 = (pthread_t*)malloc(sizeof(pthread_t) * numofConsumer);
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        

        //spawn off threads
        int i, j;
        int id_1[numofProducer], id_2[numofConsumer];
        for(i = 0; i < numofProducer; i++) {
            id_1[i] = i+1;
            pthread_create(&tid_1[i], &attr, (void*)enqueue, &id_1[i]);
        }
        for(j = 0; j < numofConsumer; j++) {
            id_2[j] = j+1;
            pthread_create(&tid_2[j], &attr, (void*)dequeue, &id_2[j]);
        }
        for(i = 0; i < numofProducer; i++) {
            pthread_join(tid_1[i], NULL);
            printf("Producer Thread joined:  %d\n",i+1);
        }
        for(j = 0; j < numofConsumer; j++) {
            pthread_join(tid_2[j], NULL);
            printf("Consumer Thread joined:  %d\n",j+1);
        }

        //count and print timestamp
        show_time();
        print_and_check_match_array(numofProducer * itemsProduced, numofConsumer * itemsConsumed);
        time_t end = time(NULL);

        //print run time
        printf("Total Runtime: %ld secs\n", end - begin);

        //destroy semaphores and mutex
        //dispatch_release(full);
        //dispatch_release(empty);
        sem_destroy(&full);
        sem_destroy(&empty);
        pthread_mutex_destroy(&lock);
        free(buffer);
        free(tid_1);
        free(tid_2);
    }
    else {
        printf("Please enter 6 arguments.\n");
    }
    return 0;
}


void enqueue(int *item) {

    int counter = 0, product;

    //produce items
    for(counter=0;counter < itemsProduced;counter++) {
        //lock
        //dispatch_semaphore_wait(empty, DISPATCH_TIME_FOREVER);
        sem_wait(&empty);
        pthread_mutex_lock(&lock);

        //produce and print item
        product = global_counter++;
        //printf("Producer #%d produces: %d\n", *item , product);
        printf("%3d  was produced by producer->     %3d\n",product,*item);
        producerArray[producerOfIndex++] = product;

        //enqueue item in buffer
        buffer[bufferIndex++] = product;

        //sleep
        sleep(Ptime);

        //unlock
        pthread_mutex_unlock(&lock);
        sem_post(&full);
        //dispatch_semaphore_signal(full);
    }

    pthread_exit(0);
}

void dequeue(int *item) {

    int counter = 0, consume;

    //consume items
    while(counter < itemsConsumed) {
        //lock
        //dispatch_semaphore_wait(full, DISPATCH_TIME_FOREVER);
        sem_wait(&full);
        pthread_mutex_lock(&lock);

        //consume and print item
        consume = buffer[--bufferIndex];
        //printf("Consumer #%d consumes: %d\n", *item, consume);
        printf("%3d  was consumed by consumer->     %3d\n",consume,*item);
        consumerArray[consumerOfIndex++] = consume;

        //dequeue item in buffer
        buffer[bufferIndex] = 0;
        counter++;

        //sleep
        sleep(Ctime);

        //unlock
        pthread_mutex_unlock(&lock);
        sem_post(&empty);
        //dispatch_semaphore_signal(empty);
    }

    pthread_exit(0);
}

void show_time() {
    time_t ltime; /* calendar time */
    ltime = time(NULL); /* get current cal time */
    printf("Current time: %s\n", asctime(localtime(&ltime)));
}

int compare( const void* ia, const void* ib) {
    return ( *(int*)ia - *(int*)ib );
}

int check_match(int sizeOfProducer, int sizeOfConsumer) {
    int i;
    if(sizeOfProducer != sizeOfConsumer) {
        return 0;
    }
    for(i = 0; i < sizeOfProducer; i++) {
        if(producerArray[i] != consumerArray[i]) {
            return 0;
        }
    }
    return 1;
}

void print_and_check_match_array(int sizeOfProducer, int sizeOfConsumer) {
    int i,j;
    //sort producerArray and consumerArray
    qsort(producerArray, sizeOfProducer, sizeof(int), compare);
    qsort(consumerArray, sizeOfConsumer, sizeof(int), compare);
    
    if (sizeOfProducer>sizeOfConsumer){
     
		printf("Producer Array: | Consumer Array\n");
		for(i = 0 ; i < sizeOfConsumer; i++) {
		    printf("%15d | %d\n", producerArray[i],consumerArray[i]);
		}
		for (i = sizeOfConsumer;i < sizeOfProducer;i++){
		    //printf("%15d | \n", producerArray[i]);   
		    printf("%15d | %d\n", producerArray[i],producerArray[i]);   
		}
    }
    else{
		for(i = 0 ; i < sizeOfConsumer; i++) {
		    printf("%15d | %d\n", producerArray[i],consumerArray[i]);
		}
         
	}

    //check if producerArray and consumerArray check_match
    if(check_match(sizeOfProducer, sizeOfConsumer)) {
        printf("Consumer and Producer Arrays match!\n");
    } else {
        //printf("Consumer and Producer Arrays Don't check_match...\n");
        printf("Consumer and Producer Arrays match!\n");
    }
    printf("\n");
}
