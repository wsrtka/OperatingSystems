
#include "common.h"


int socket_fd;


void open_server(char* path){

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        error("Could not create socket.");
    }
    printf("Socket succsessfully created.\n");

    if(bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1){
        error("Could not bind socket.");
    }
    printf("Socket bound.\n");

    if(listen(socket_fd, MAX_CLIENTS) == -1){
        error("Could not start listening for connections.");
    }
    printf("Started listening for client connections.\n");

}

void close_server(){

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

    printf("Server shutdown.\n");

    exit(EXIT_SUCCESS);

}


void* ping_manager_f(void* args);


int main(int argc, char* argv[]){

    atexit(close_server);
    signal(SIGINT, close_server);


    if(argc != 3){
        error("Invalid number of arguments.");
    }

    char* socket_path = argv[2];


    open_server(socket_path);


    pthread_t ping_manager;

    pthread_create(ping_manager, NULL, ping_manager_f, NULL);


    close_server();


    return 0;

}