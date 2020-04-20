#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <signal.h>

int queue_id;

void sigint_handler(){

}

int main(){
    signal(SIGINT, sigint_handler);

    char file_path[PATHLENGTH];
    realpath("server.c", file_path);
    key_t obj_key = ftok(file_path, PROJECT);

    if((queue_id = msgget(obj_key, IPC_CREAT)) == -1){
        printf("Error encountered in queue creation.\n");
        exit(EXIT_FAILURE);
    }
    
    

    return 0;
}
