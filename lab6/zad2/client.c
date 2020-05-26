#define _POSIX_C_SOURCE 199309
#define _GNU_SOURCE

#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>

int client_id, active = 0;
char* queue_name;
mqd_t client_queue, server_queue, connected = -1;

void sigint_handler(){
    char* msg;
    sprintf(msg, "%d", client_id);

    if(mq_send(server_queue, msg, strlen(msg), STOP) == -1){
        printf("Failed to send stop request.\n");
        return;
    }
    printf("Stop request sent to server.\n");

    if(mq_close(client_queue) == -1){
        printf("Failed to close client queue %d.\n", server_queue);
    }

    if(mq_close(client_queue) == -1){
        printf("Failed to close server queue %d.\n", server_queue);
    }

    if(mq_unlink(queue_name) == -1){
        printf("Failed to remove queue with name %s.\n", queue_name);
    }
}

void initialize(){
    printf("Initializing client...\n");

    signal(SIGINT, sigint_handler);
    printf("Signal handlers set.\n");

    char* server_queue_name;
    sprintf(server_queue_name, "/%s", (char)PROJECT);
    server_queue = mq_open(server_queue_name, O_RDWR);

    int pid = getpid();
    sprintf(queue_name, "/%d", pid);
    if((client_queue = mq_open(queue_name, O_RDWR | O_CREAT)) == -1){
        error("Could not create client queue.\n");
    }

    if(mq_send(server_queue, queue_name, strlen(queue_name), INIT) == -1){
        error("Failed to send queue name to server.\n");
    }
    printf("Queue name sent to server.\n");

    int prio;

    if(mq_receive(client_queue, server_queue_name, MSG_SIZE, &prio) == -1){
        error("Could not get client id.\n");
    }
    printf("Client id received.\n");

    printf("Client initialized.\n");
}

void list(){
    char* msg;
    sprintf(msg, "%d", client_id);
    if(mq_send(server_queue, msg, strlen(msg), LIST) == -1){
        error("Failed to send list request to server.\n");
    }

    int priority;
    printf("Clients available: \n");
    do{
        mq_receive(client_queue, msg, MSG_SIZE, &priority);
        if(priority == LIST){
            printf("\t%s\n", msg);
        }

    }while(strcmp(msg, "end") != 0);
}

void disconnect(){
    if(mq_send(server_queue, "end", 10, DISCONNECT) == -1 || mq_send(connected, "end", 10, DISCONNECT) == -1){
        error("Failed to send disconnect request.\n");
    }

    active = 0;
    connected = -1;
}

void receive_msg(){
    char* text;
    int prio;

    mq_receive(client_queue, text, MSG_SIZE, &prio);
    if(prio == STOP){
        raise(SIGINT);
    }
    if(prio == DISCONNECT){
        disconnect();
    }
    if(prio == CONNECT){
        printf("%s\n", text);
    }

    struct sigevent rec;
    rec.sigev_notify = SIGEV_THREAD;
    rec.sigev_notify_function = receive_msg;
    mq_notify(client_queue, &rec);
}

void chat(char* id){
    connected = atoi(id);
    char* text;

    struct sigevent rec;
    rec.sigev_notify = SIGEV_THREAD;
    rec.sigev_notify_function = receive_msg;
    mq_notify(client_queue, &rec);

    while(active){
        scanf("%s", text);

        if(strcmp(text, "/disconnect") == 0){
            printf("Are you sure? [y/n]\n");
            scanf("%s", text);

            if(strcmp(text, "y") == 0){
                disconnect();
                break;
            }
        }
        else{
            if(mq_send(connected, text, strlen(text), CONNECT) == -1){
                printf("Failed to send message.\n");
            }
        }
    }
}

void connect_with(char* id){
    char* msg;
    sprintf(msg, "%d %s", client_id, id);

    if(mq_send(server_queue, msg, strlen(msg), CONNECT) == -1){
        printf("Failed to connect with client %s.\n", id);
        return;
    }
    printf("Connection request sent.\n");

    int prio;
    if(mq_receive(client_queue, msg, MSG_SIZE, &prio) == -1){
        printf("Failed to receive connection key.\n");
        return;
    }
    if(strcmp(msg, "failed") == 0){
        printf("Client is not available for connection.\n");
        return;
    }
    printf("Connection key received.\n");

    chat(msg);
}

void handle_command(char* str){
    str = strtok(str, " ");

    if(strcmp(str, "list") == 0){
        list();
    }
    else if(strcmp(str, "connect") == 0){
        char* connect_id = strtok(NULL, " ");
        connect_with(connect_id);
    }
    else if(strcmp(str, "stop") == 0){
        raise(SIGINT);
        exit(EXIT_SUCCESS);
    }
    else{
        printf("Could not understand command. Try using one of these:\n");
        printf("\tlist\n\tconnect [client_id]\n\tdisconnect\n\tstop\n");
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