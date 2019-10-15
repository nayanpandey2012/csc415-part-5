#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

/**
 * THESE DEFINE VALUES CANNOT BE CHANGED.
 * DOING SO WILL CAUSE POINTS TO BE DEDUCTED
 * FROM YOUR GRADE
 */
 /** BEGIN VALUES THAT CANNOT BE CHANGED */
#define MAX_THREADS 36
#define MAX_ITERATIONS 22 // Max iterations per thread
/** END VALUES THAT CANNOT BE CHANGED */

// declare a single shared integer variable (i.e. a global variable) with initial value zero

int globalVariable = 0;
pthread_mutex_t mid;

/**
 * use this struct as a parameter for the
 * nanosleep function.
 * For exmaple : nanosleep(&ts, NULL);
 */
struct timespec ts = {0, 150000};
// //Function for threads created on even numbered iterations as adders

void* adderThread(void *tid)

{
     pthread_mutex_lock(&mid); //here mutex is locked by the thread to avoid race condition
     int tId = *((int*)tid);

     int it;

     // Declare temp

     int temp;

     for (it = 0; it < MAX_ITERATIONS; it++) {

          // sleeping for some time

          // Not sure where to keep the delay.

          nanosleep(&ts, NULL);

          // reading global value.

          temp = globalVariable;

          // adding 10 to temp

          temp = temp + 10;

          //store temp value into global variable.

          globalVariable = temp;

          printf("Current Value written to Global Variables by thread : %d is %d ", tId, temp);

     }
     pthread_mutex_unlock(&mid);    //thread will unlock mutex when finish


     return NULL;

}

//Function for threads created on odd iterations as subtractors.

void* subtractorThread(void *tid) {
    pthread_mutex_lock(&mid); //here mutex is locked by the thread to avoid race condition

     int tId = *((int*)tid);

     int it;

     // reading global value and store it into temp

     int temp = globalVariable;

     for (it = 0; it < MAX_ITERATIONS; it++) {

          nanosleep(&ts, NULL);

          // reading global value

          int temp = globalVariable;

          // subtracting 10

          temp = temp - 10;

          // store temp value into global variable

          globalVariable = temp;

          printf("Current Value written to Global Variables by thread : %d is %d ", tId, temp);

     }
     pthread_mutex_unlock(&mid);    //thread will unlock mutex when finish

     return NULL;

}

int main()

{

     int p;

     int err;

     // array for threads.

     pthread_t threads[MAX_THREADS + 1];
     pthread_mutex_init(&mid,NULL);

     // for-loop to create Maximum threads

     for (p = 1; p <= MAX_THREADS; p++)

{

          // even thread should be adder thread.

          if (p % 2 == 0) {

              err = pthread_create(&threads[p], NULL, adderThread, (void*)&p);

              if (err) {

                   printf("ERROR creating THREADS, return code %d ", err);

                   exit(-1);

              }

          }

          // odd thread should be subtract thread.

          else {

              err = pthread_create(&threads[p], NULL, subtractorThread, (void*)&p);

              if (err) {

                   printf("ERROR creating THREADS, return code %d ", err);

                   exit(-1);

              }

          }

     }

     // Waiting for all threads to finish.

     for (p = 1; p <= MAX_THREADS; p++) {

          pthread_join(threads[p], NULL);

     }

     // printing the final global value.

     printf(" Final Value of Shared Variable : %d ", globalVariable);

     return 0;

}