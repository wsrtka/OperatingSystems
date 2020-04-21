#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <signal.h>

int queue_id, size = 0;
int client_qids[MAXCLIENTS];
key_t client_keys[MAXCLIENTS];
int client_available[MAXCLIENTS];

int find_new_place(){
    for(int i = size; i < size + MAXCLIENTS; i++){
        if(client_qids[i % MAXCLIENTS] == -1){
            return i;
        }
    }

    return -1;
}

int find_id_by_key(key_t key){
    for(int i = 0; i < MAXCLIENTS; i++){
        if(client_keys[i] == key){
            return i;
        }
    }

    return -1;
}

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
        disconnect_client(msg.obj_id);
        break;

    case INIT:             
        init_client(msg.obj_key);
        break;

    case LIST:             
        list_clients(msg.obj_id);
        break;

    case CONNECT:             
        connect_clients(msg.obj_id, atoi(msg.mtext));
        break;

    default:
        error("Invalid msg type.\n");
    }
}

void init_client(int object_key){
    if(MAXCLIENTS <= size){
        return;
    }

    int id = find_new_place();
    if((client_qids[id] = msgget(object_key, IPC_CREAT)) == -1){
        return;
    }

    client_keys[id] = object_key;
    client_available[id] = 1;
    struct msgbuf msg;
    msg.obj_id = id;
    msg.mtype = INIT;

    if(msgsnd(client_qids[id], &msg, MSG_SIZE, 0) == -1){
        error("Could not send respond to client.\n");
    }
}

void list_clients(int client_id){
    struct msgbuf list_res;
    list_res.mtype = LIST;

    for(int i = 0; i < MAXCLIENTS; i++){
        if(client_qids != -1 && client_available){
            list_res.obj_id = i;
            msgsnd(client_qids[client_id], &list_res, MSG_SIZE, 0);
        }
    }
}

void connect_clients(int client_src, int client_dest){
    struct msgbuf connect_res;
    connect_res.mtype = CONNECT;
    connect_res.obj_key = -1;

    if(client_available[client_src] == 0 || client_available[client_dest] == 0){
        msgsnd(client_qids[client_src], &connect_res, MSG_SIZE, 0);
        return;
    }

    connect_res.obj_key = client_keys[client_dest];
    if(msgsnd(client_qids[client_src], &connect_res, MSG_SIZE, 0) == -1){
        printf("Could not connect client.\n");
        return;
    }

    connect_res.obj_key = client_keys[client_src];
    if(msgsnd(client_qids[client_dest], &connect_res, MSG_SIZE, 0) == -1){
        printf("Could not connect client.\n");
        return;
    }

    client_available[client_dest] = 0;
    client_available[client_src] = 0;
}

void disconnect_client(int client_id){
    client_available[client_id] = 1;
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
