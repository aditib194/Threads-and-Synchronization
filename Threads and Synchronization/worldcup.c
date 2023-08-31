#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

static sem_t connected_lock;
static sem_t operators;
static int NUM_OPERATORS = 3;
static int NUM_LINES = 5;
static int connected = 0;

static int next_id = 0;

void *phonecall(void *vargp) {
    int ret;

    sem_wait(&connected_lock);
    int id = ++next_id;
    sem_post(&connected_lock);
    
    //Print that an attempt to connect has been made.
    printf("Thread %d is attempting to connect to the conference center ...\n", id);

    sem_wait(&connected_lock);
    //Check if the connection can be made:
    while(connected == NUM_LINES) {
        //line is busy
        sem_post(&connected_lock);
        //try again in 1 second
        sleep(1);
        sem_wait(&connected_lock);
    }
    //line is not busy
    //update connected
    connected++;
    sem_post(&connected_lock);
    printf("Thread %d connects to an available line, call ringing ...\n", id);
    
    //Wait for an operator to be available
    sem_wait(&operators);
    //Print a message that the question is being taken by an operator.
    printf("Thread %d is speaking to an operator.\n", id);
    //Simulate a question proposal by sleeping for 1 second.
    sleep(1);
    //Print a message that the question proposal is complete (and update the semaphore).
    printf("Thread %d has proposed a question for the USA soccer team! The operator has left...\n", id);
    sem_post(&operators);

    //Update conencted (using a binary semaphore).
    sem_wait(&connected_lock);
    connected--;
    sem_post(&connected_lock);

    //Print a message that the call is over.
    printf("Thread %d has hung up!\n", id);
    ret = pthread_detach(pthread_self());
    if(ret != 0) {
        exit(EXIT_FAILURE);
    }
}

//parameter refers to seconds passed in main function which specifies how long to block the main thread for
void *thread(void *vargp) {
    int ret;

    sleep(*((int *) (vargp)));
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    //initialie semaphores
    int ret;
    sem_init(&operators,0,NUM_OPERATORS);
    sem_init(&connected_lock,0,1);

    pthread_t tids[100];
    pthread_t timer;

    int seconds;
    //Manage user input correctly, including no input (argc == 1) or irregular user input (atoi returns 0)
    if(argc == 1) {
        seconds = 3;
    } else {
        if(atoi(argv[1]) == 0) {
            seconds = 3;
        } else {
        seconds = atoi(argv[1]);
        }
    }

    for(int i = 0; i < 100; i++) {
        ret = pthread_create(tids + i, NULL, phonecall, NULL);
        if(ret != 0) {
            exit(EXIT_FAILURE);
        }
    }

    ret = pthread_create(&timer, NULL, thread, (void *) (&seconds));
    if(ret != 0) {
        exit(EXIT_FAILURE);
    }
    ret = pthread_join(timer, NULL);
    if(ret != 0) {
        exit(EXIT_FAILURE);
    }

    sem_destroy(&connected_lock);
    sem_destroy(&operators);
    
    exit(0);
}