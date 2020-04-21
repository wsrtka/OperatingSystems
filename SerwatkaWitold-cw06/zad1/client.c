#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int server_queue, client_queue, client_id;

void sigint_handler(){

}

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

    if((client_queue = msgget(object_key, 0)) == -1){
        error("Could not get client queue.\n");
    }
    printf("Client queue set.\n");

    if(msgrcv(client_queue, &init_msg, MSG_SIZE, INIT, 0) == -1){
        error("Could not get client id.\n");
    }
    client_id = init_msg.obj_id;
    printf("Client id revceived.\n");

    printf("Client initialized.\n");
}

void handle_command(char* str){
    str = strtok(str, " ");

    if(strcmp(str, "list") == 0){
        list();
    }
    else if(strcmp(str, "connect") == 0){

    }
    else if(strcmp(str, "disconnect") == 0){

    }
    else if(strcmp(str, "stop") == 0){

    }
    else{
        printf("Could not understand command. Try using one of these:\n");
        printf("\tlist\n\tconnect [client_id]\n\tdisconnect\n\tstop\n");
    }
}

void list(){
    struct msgbuf list_req;
    list_req.mtype = LIST;
    list_req.obj_id = client_id;

    if(msgsnd(server_queue, &list_req, MSG_SIZE, 0) == -1){
        error("Failed to send list request to server.\n");
    }

    printf("Clients available: \n");
    while(msgrcv(client_queue, &list_req, MSG_SIZE, LIST, 0) > 0){
        printf("%d\n", list_req.obj_id);
    }
}

int main(){
    initialize();

    char command[32];

    while(1){
        printf("> ");
        scanf("%s", command);
        handle_command(command);
    }

    return 0;
}