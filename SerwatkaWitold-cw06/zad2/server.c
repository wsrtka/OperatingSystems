#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

int size = 0;
mqd_t server_queue;
char* queue_name;
mqd_t client_qids[MAXCLIENTS];
int client_available[MAXCLIENTS];

int find_new_place(){
    for(int i = size; i < size + MAXCLIENTS; i++){
        if(client_qids[i % MAXCLIENTS] == -1){
            return i;
        }
    }

    return -1;
}

void sigint_handler(){
    pid_t pid = fork();
    char* msg = "stop";

    if(pid == 0){
        for(int i = 0; i < MAXCLIENTS; i++){
            if(client_qids[i] != -1){
                mq_send(client_qids[i], msg, strlen(msg), STOP);
                printf("Sent STOP to client number %d\n", i);
            }
        }

        exit(EXIT_SUCCESS);
    }
    else if(pid > 0){
        while(size > 0){
            mq_receive(server_queue, msg, MSG_SIZE, STOP);
            client_qids[atoi(msg)] = -1;
            size--;
            printf("Received STOP signal from client number %s.\n", msg);
        }
    }
    else{
        error("Error while shutting down server.\n");
    }

    if(mq_close(server_queue) == -1){
        printf("Failed to close queue %d.\n", server_queue);
    }

    if(mq_unlink(queue_name) == -1){
        printf("Failed to remove queue with name %s.\n", queue_name);
    }

    printf("Server shutdown .\nGoodbye\n");
}

void initialize(){
    printf("Initializing server...\n");

    signal(SIGINT, sigint_handler);

    printf("Signal handler set.\n");

    int pid = getpid();
    sprintf(queue_name, "%d", pid);

    if((server_queue = mq_open(queue_name, O_RDWR | O_CREAT)) != -1){
        error("Could not create server queue.\n");
    }
    printf("Server queue created.\n");

    for(int i = 0; i < MAXCLIENTS; i++){
        client_qids[i] = -1;
    }

    printf("Server initialized.\n");
}

void init_client(char* msg){
    if(MAXCLIENTS < size){
        return;
    }

    int id = find_new_place();
    client_qids[id] = mq_open(msg, O_RDWR);
    client_available[id] = 1;

    if(mq_send(client_qids[id], "success", 10, INIT) == -1){
        error("Could not send respond to client.\n");
    }
}

void list_clients(char* msg){
    int id = atoi(msg);
    char* res;

    for(int i = 0; i < MAXCLIENTS; i++){
        if(client_qids[i] != -1 && client_available[i]){
            sprintf(res, "%d", i);
            mq_send(client_qids[id], res, strlen(res), LIST);
        }
    }
}

void connect_clients(char* msg){
    int client_src = atoi(strtok(msg, " "));
    int client_dest = atoi(strtok(NULL, " "));

    if(client_available[client_src] == 0 || client_available[client_dest] == 0){
        mq_send(client_qids[client_src], "failed", 10, CONNECT);
        return;
    }

    char* res;
    sprintf(res, "%d", client_qids[client_dest]);
    if(mq_send(client_qids[client_dest], res, strlen(res), CONNECT) == -1){
        printf("Could not connect client.\n");
        return;
    }

    sprintf(res, "%d", client_qids[client_src]);
    if(mq_send(client_qids[client_src], res, strlen(res), CONNECT) == -1){
        printf("Could not connect client.\n");
        return;
    }

    client_available[client_dest] = 0;
    client_available[client_src] = 0;
}

void disconnect_client(char* msg){
    client_available[atoi(msg)] = 1;
}

void handle_msg(char* msg_ptr, int msg_prio){
    switch (msg_prio)
    {
    case STOP:             
        raise(SIGINT);
        exit(EXIT_SUCCESS);
        break;
    
    case DISCONNECT:             
        disconnect_client(msg_ptr);
        break;

    case INIT:             
        init_client(msg_ptr);
        break;

    case LIST:             
        list_clients(msg_ptr);
        break;

    case CONNECT:             
        connect_clients(msg_ptr);
        break;

    default:
        error("Invalid msg type.\n");
    }
}

int main(){
    initialize();

    int msg_prio;
    char* msg_ptr;

    while(1){
        if(mq_receive(server_queue, msg_ptr, MSG_SIZE, msg_prio) == -1){
            error("Could not receive message.\n");
        }

        handle_msg(msg_ptr, msg_prio);
    }

    return 0;
}
