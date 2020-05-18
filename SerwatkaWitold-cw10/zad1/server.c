#include "common.h"

in_port_t inet_port;
char* socket_path;
int socket_fd;

client clients[MAX_CLIENTS];

pthread_mutex_t mut_clients = PTHREAD_MUTEX_INITIALIZER;

void setup_server(in_port_t port, char* path, int* socket_fd){

    struct sockaddr_un addr;
    strcpy(addr.sun_path, path);
    addr.sun_family = AF_UNIX;

    if((*socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        error("Could not create unix socket.");
    }

    if(bind(*socket_fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) != 0){
        error("Could not bind socket.");
    }

    if(listen(*socket_fd, MAX_CLIENTS) != 0){
        error("Could not start accepting client connection requests.");
    }

}

void close_server(){
    if(socket_fd != -1){
        if(close(socket_fd) != 0){
            printf("Unable to close socket.\n");
        }

        if(unlink(socket_path) != 0){
            printf("Unable to unlink socket.\n");
        }
    }
}

int main(int argc, char* argv[]){
    
    atexit(close_server);
    signal(SIGINT, close_server);

    if(argc != 3){
        error("Invalid number of arguments.");
    }

    inet_port = (in_port_t) atoi(argv[1]);
    socket_path = argv[2];

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].active = 0;
        clients[i].name = "\0";
        clients[i].registered = 0;
        clients[i].socket_fd = -1;
    }

    setup_server(inet_port, socket_path, &socket_fd);

    return 0;
}