#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <signal.h>

int queue_id;

void sigint_handler(){

}

void initialize(){
    printf("Initializing server...\n");

    signal(SIGINT, sigint_handler);

    printf("Signal handler set.\n");

    char* file_path;   
    if((file_path = getenv("HOME")) == -1){
        error("Error getting file path.\n");
    }
    printf("File path aquired.\n"); 

    key_t obj_key;
    if((obj_key = ftok(file_path, PROJECT)) == -1){
        error("Error getting object key.\n");
    }
    printf("Object key aquired.\n");

    if((queue_id = msgget(obj_key, IPC_CREAT)) == -1){
        error("Error encountered in queue creation.\n");
    }
    
    printf("Initialized new message queue with id %d\n", queue_id);
    printf("Server initialized.\n");
}

int main(){
    initialize();

    while(1){
        //obsługa komunikatów
    }

    return 0;
}
