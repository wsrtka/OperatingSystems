#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <signal.h>

int queue_id;

void sigint_handler(){

}

int main(){
    printf("Initializing server...\n");

    signal(SIGINT, sigint_handler);

    printf("Signal handler set.\n");

    char* file_path;   
    if((file_path = getenv("HOME")) == -1){
        printf("Error getting file path.\n");
        exit(EXIT_FAILURE);
    }

    key_t obj_key;
    if((obj_key = ftok(file_path, PROJECT)) == -1){
        printf("Error getting object key.\n");
        exit(EXIT_FAILURE);
    }

    printf("Object key aquired.\n");

    if((queue_id = msgget(obj_key, IPC_CREAT)) == -1){
        printf("Error encountered in queue creation.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Initialized new message queue with id %d\n", queue_id);
    printf("Server initialized.\n");

    while(1){
        //obsługa komunikatów
    }

    return 0;
}
