#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

void error(char* msg){
    printf("Error: %s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
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

    pthread_t barber;
    pthread_t customer[CUSTOMERS];

    if(pthread_create(&barber, NULL, barber, NULL) != 0){
        error("Could not initialize barber.");
    }

    srand(time(NULL));

    for(int i = 0; i < CUSTOMERS; i++){
        sleep((rand() % 10) + 1);

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