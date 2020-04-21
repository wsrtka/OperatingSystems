#include "header.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int server_queue, client_queue, client_id, connected = -1;

void sigint_handler(){
    struct msgbuf stop_req;
    stop_req.mtype = DISCONNECT;
    stop_req.obj_id = client_id;

    if(msgsnd(server_queue, &stop_req, MSG_SIZE, 0) == -1){
        printf("Failed to send stop request.\n");
        return;
    }
    printf("Stop request sent to server.\n");

    msgctl(client_queue, IPC_RMID, NULL);
}

void initialize(){
    printf("Initializing client...\n");

    signal(SIGINT, sigint_handler);
    // if(atexit(sigint_handler) == -1){
    //     error("Failed to set exit function.\n");
    // }
    printf("Signal handlers set.\n");

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
        int connect_id = strtok(NULL, " ");
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

void connect_with(char* id){
    struct msgbuf connect_req;

    connect_req.mtype = CONNECT;
    strcpy(connect_req.mtext, id);
    connect_req.obj_id = client_id;

    if(msgsnd(server_queue, &connect_req, MSG_SIZE, 0) == -1){
        printf("Failed to connect with client %s.\n", id);
        return;
    }
    printf("Connection request sent.\n");

    if(msgrcv(client_queue, &connect_req, MSG_SIZE, CONNECT, 0) == -1){
        printf("Failed to receive connection key.\n");
        return;
    }
    if(connect_req.obj_key == -1){
        printf("Client is not available for connection.\n");
        return;
    }
    printf("Connection key received.\n");

    chat(connect_req.obj_key);
}

void chat(key_t key){
    if((connected = msgget(key, 0)) == -1){
        printf("Could not initialize chat.\n");
        return;
    }

    pid_t chat = fork();
    pid_t parent_pid = getpid();

    if(chat == 0){
        struct msgbuf chat_read;

        while(1){
            msgrcv(connected, &chat_read, MSG_SIZE, 0, 0);
            if(chat_read.mtype == CONNECT){
                printf("%s\n", chat_read.mtext);
            }

            if(chat_read.mtype == DISCONNECT){
                printf("Your partner disconnected.\n");
                exit(EXIT_SUCCESS);
            }
        }
    }
    else if(chat > 0){
        struct msgbuf chat_send;
        chat_send.mtype = CONNECT;
        char text[MSG_SIZE];

        while(1){
            scanf("%s", text);

            if(kill(0, chat) == -1){
                disconnect(key);
                break;
            }

            if(strcmp(text, "/disconnect") == 0){
                printf("Are you sure? [y/n]\n");
                scanf("%s", text);

                if(strcmp(text, "y") == 0){
                    disconnect(key);
                    break;
                }
            }
            else{
                strcpy(chat_send.mtext, text);
                
                if(msgsnd(connected, &chat_send, MSG_SIZE, 0) == -1){
                    printf("Failed to send message.\n");
                }
            }
        }
    }
    else{
        printf("Could not initialize chat.\n");
        return;
    }
}

void disconnect(int key){
    struct msgbuf disconnect_req;
    disconnect_req.mtype = DISCONNECT;
    disconnect_req.obj_key = key;
    disconnect_req.obj_id = client_id;

    if(msgsnd(server_queue, &disconnect_req, MSG_SIZE, 0) == -1 || msgsnd(connected, &disconnect_req, MSG_SIZE, 0) == -1){
        error("Failed to send disconnect request.\n");
    }

    connected = -1;
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