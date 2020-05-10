#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

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

    
}