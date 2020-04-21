#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <signal.h>

int queue_id, size = 0;
int client_qids[MAXCLIENTS];
key_t client_keys[MAXCLIENTS];

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

    for(int i = 0; i < MAXCLIENTS; i++){
        client_qids[i] = -1;
    }

    printf("Server initialized.\n");
}

void handle_msg(struct msgbuf msg){
    switch (msg.mtype)
    {
    case STOP:             
        /* code */
        break;
    
    case DISCONNECT:             

        break;

    case INIT:             
        init_client(msg.obj_key);
        break;

    case LIST:             

        break;

    case CONNECT:             

        break;

    default:
        error("Invalid msg type.\n");
    }
}

void init_client(int object_key){
    if(MAXCLIENTS <= size){
        return;
    }

    
}

int main(){
    initialize();

    struct msgbuf msg;
    while(1){
        if(msgrcv(queue_id, &msg, MSG_SIZE, -4, 0) == -1){
            error("Could not receive message.\n");
        }

        handle_msg(msg);
    }

    return 0;
}
