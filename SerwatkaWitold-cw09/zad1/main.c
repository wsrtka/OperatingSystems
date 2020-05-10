#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define MAXTIME 10

pthread_mutex_t chair;
pthread_mutex_t* waiting_room; 
int waiting;
pthread_cond_t has_customers;
pthread_cond_t is_working;

void error(char* msg){
    printf("Error: %s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void initialize_mutex(pthread_mutex_t* mutex){
    pthread_mutexattr_t attr;

    if(pthread_mutexattr_init(&attr) != 0){
        error("Could not initialize chair mutex attributes.");
    }

    if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0){
        error("Could not set chair mutex mode.");
    }

    if(pthread_mutex_init(&mutex, &attr) != 0){
        error("Could not initialize chair mutex.");
    }
}

void* barber(void* args){

}

void* customer(void* args){

}

int main(int argc, char* argv[]){
    if(argc != 3){
        error("Invalid number of arguments.");
    }

    const int CHAIRS = atoi(argv[1]);
    const int CUSTOMERS = atoi(argv[2]);

    waiting_room = calloc(CHAIRS, sizeof(pthread_mutex_t));

    pthread_t barber;
    pthread_t customer[CUSTOMERS];

    initialize_mutex(&chair);

    for(int i = 0; i < CHAIRS; i++{
        initialize_mutex(&waiting_room[i]);
    }

    if(pthread_create(&barber, NULL, barber, NULL) != 0){
        error("Could not initialize barber.");
    }

    srand(time(NULL));

    for(int i = 0; i < CUSTOMERS; i++){
        sleep((rand() % MAXTIME) + 1);

        if(pthread_create(&customer[i], NULL, customer, NULL) != 0){
            error("Could not create customer.");
        }
    }

    void* retval;
    for(int i = 0; i < CUSTOMERS; i++){
        if(pthread_join(customer[i], &retval) != 0){
            printf("Could not join thread no %d.\n", i);
        }
        else{
            printf("Thread no %d joined.\n", i);
        }
    }

    if(pthread_join(barber, &retval) != 0){
        printf("Could not join barber thread.\n");
    }
    else{
        printf("Barber thread joined.\n");
    }

    return 0;
}