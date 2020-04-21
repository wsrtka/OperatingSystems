#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int server_queue, client_queue;

void initialize(){
    printf("Initializing client...\n");

    char* file_path;
    if((file_path = getenv("HOME")) == -1){
        error("Error getting file path.\n");
    }
    printf("File path aquired.\n");

    key_t server_key;
    if((server_key = ftok(file_path, PROJECT)) == -1){
        error("Could not obtain server key.\n");
    }
    printf("Server key obtained.\n");

    if((server_queue = msgget(server_key, 0)) == -1){
        error("Could not connect to server queue.\n");
    }
    printf("Connected to server queue.\n");

    key_t object_key;
    if((object_key = ftok(file_path, getpid())) == -1){
        error("Could not get client key.\n");
    }
    printf("Obtained client key.\n");

    struct msgbuf init_msg;
    init_msg.mtype = INIT;
    init_msg.obj_key = object_key;
    if(msgsnd(server_queue, &init_msg, MSG_SIZE, 0) == -1){
        error("Failed to send object key to server.\n");
    }
    printf("Object key sent to server.\n");

    if((client_queue = msget(object_key, IPC_CREAT)) == -1){
        error("Could not create client queue.\n");
    }
    printf("Client queue created.\n");

    printf("Client initialized.\n");
}

int main(){
    initialize();

    return 0;
}