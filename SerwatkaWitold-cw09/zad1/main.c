#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAXTIME 3

pthread_mutex_t chair;
int waiting;
pthread_cond_t has_customers;
pthread_cond_t finished_working;
pthread_cond_t waiting_room_full;
pthread_cond_t was_sleeping;

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

    if(pthread_mutex_init(mutex, &attr) != 0){
        error("Could not initialize chair mutex.");
    }
}

void initialize_cond(pthread_cond_t* cond){
    pthread_condattr_t attr;

    if(pthread_condattr_init(&attr) != 0){
        error("Could not initialize condition attribute.");
    }

    if(pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0){
        error("Could not set condition type.");
    }

    if(pthread_cond_init(cond, &attr) != 0){
        error("Could not initialize condition variable.");
    }
}

void* barber_fun(void* args){
    int id = 0;

    while(1){
        if(waiting == 0){
            printf("Golibroda idzie spać.\n");
            pthread_mutex_lock(&chair);
            pthread_cond_wait(&has_customers, &chair);
            pthread_mutex_unlock(&chair);
            printf("Komunikat: budzę golibrodę.\n");
        }
        
        printf("Golibroda: czeka %d klientów, golę klienta nr %d.\n", waiting--, id++);

        sleep((rand() % (3 * MAXTIME)) + 1);

        pthread_mutex_lock(&chair);
        pthread_cond_signal(&finished_working);
        pthread_mutex_unlock(&chair);
    }
}

void* customer_fun(void* args){
    int* chairs = (int*) args;

    pthread_mutex_lock(&chair);
    if(waiting == *chairs){
        printf("Komunikat: zajęte.\n");
        pthread_cond_wait(&waiting_room_full, &chair);
    }
    pthread_mutex_unlock(&chair);

    waiting++;
    printf("Komunikat: poczekalnia, wolne miejsca: %d.\n", *chairs - waiting);

    pthread_mutex_lock(&chair);
    pthread_cond_broadcast(&has_customers);
    pthread_mutex_unlock(&chair);

    pthread_mutex_lock(&chair);
    pthread_cond_wait(&finished_working, &chair);
    pthread_cond_signal(&waiting_room_full);
    pthread_mutex_unlock(&chair);

    pthread_exit(0);
}

int main(int argc, char* argv[]){
    if(argc != 3){
        error("Invalid number of arguments.");
    }

    const int CHAIRS = atoi(argv[1]);
    const int CUSTOMERS = atoi(argv[2]);

    pthread_t barber;
    pthread_t customer[CUSTOMERS];

    initialize_mutex(&chair);

    initialize_cond(&finished_working);
    initialize_cond(&has_customers);
    initialize_cond(&waiting_room_full);

    if(pthread_create(&barber, NULL, barber_fun, NULL) != 0){
        error("Could not initialize barber.");
    }

    srand(time(NULL));

    for(int i = 0; i < CUSTOMERS; i++){
        sleep((rand() % MAXTIME) + 1);

        if(pthread_create(&customer[i], NULL, customer_fun, (void*) &CHAIRS) != 0){
            error("Could not create customer.");
        }
    }

    void* retval;
    for(int i = 0; i < CUSTOMERS; i++){
        if(pthread_join(customer[i], &retval) != 0){
            printf("Could not join thread no %d.\n", i);
        }
        else{
            // printf("Thread no %d joined.\n", i);
        }
    }

    return 0;
}