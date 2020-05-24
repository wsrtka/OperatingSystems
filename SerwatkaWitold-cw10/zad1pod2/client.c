
#include "common.h"


int socket_fd;


void open_client(char* path, char* name){

    struct sockaddr_un addr;

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        error("Could not create socket.");
    }
    printf("Socket successfully created.\n");

    if(connect(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1){
        error("Could not connect to server.");
    }
    printf("Connected to server.\n");


    char msg[MSG_SIZE];

    if(read(socket_fd, msg, MSG_SIZE) < 1){
        error("Could not read server response.");
    }
    
    if(msg[0] == REJECT){
        error("Server full.");
    }


    strcpy(msg, name);

    if(write(socket_fd, msg, sizeof(msg)) < 1){
        error("Could not send client name.");
    }
    printf("Client name sent.\n");

    if(read(socket_fd, msg, MSG_SIZE) < 1){
        error("Could not read server response.");
    }

    if(msg[0] == REJECT){
        error("Client with choosen name already exists.");
    }
    printf("Received name confirmation from server.\n");

}

void close_client(){

    if(socket_fd != -1){

        if(shutdown(socket_fd, SHUT_RDWR) == -1){
            printf("Could not shutdown socket.\n %s\n", strerror(errno));
        }
        else{
            printf("Socket shutdown.\n");
        }

        if(close(socket_fd) == -1){
            printf("Could not close socket.\n %s\n", strerror(errno));
        }
        else{
            printf("Socket closed.\n");
        }

    }

    printf("Client shutdown.\n");

}


int main(int argc, char* argv[]){

    atexit(close_client);
    signal(SIGINT, close_client);


    char* name;
    char* connection_type;
    char* server_path;

    if(argc == 4){

        name = argv[1];
        connection_type = argv[2];
        server_path = argv[3];
        
    }
    else if(argc == 5){
        //define settings for ITNET connection
    }
    else{
        error("Invalid number of arguments.");
    }


    open_client(server_path, name);


    sleep(5);


    close_client();


    return 0;

}