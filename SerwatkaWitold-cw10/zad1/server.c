#include "common.h"

in_port_t inet_port;
char* socket_path;
int socket_fd;

client clients[MAX_CLIENTS];

pthread_mutex_t mut_clients = PTHREAD_MUTEX_INITIALIZER;


//===========SERVER FUNCTIONS=============//
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

//===========THREAD FUNCTIONS==============//
void* connection_manager_f(void* args){

    while(1){

        struct sockaddr_un client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int i, new_client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len);

        pthread_mutex_lock(&mut_clients);

        for(i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].socket_fd == -1){
                clients[i].socket_fd = new_client_fd;
                clients[i].registered = 1;

                write(new_client_fd, "Client registered.", 18);
                read(new_client_fd, clients[i].name, NAME_LEN);
            }
        }

        if(i == MAX_CLIENTS){
            printf("Could not register new client, already reached max client number.\n");
        }

        pthread_mutex_unlock(&mut_clients);

    }

    return 0;
}

void* ping_manager_f(void* args){

    while(1){

        sleep(3);

        pthread_mutex_lock(&mut_clients);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].registered == 1){
                
            }

        }

    }

    return 0;
}

void* game_manager_f(void* args){
    return 0;
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
        bzero((char*) clients[i].name, sizeof(clients[i].name));
        clients[i].registered = 0;
        clients[i].socket_fd = -1;
    }

    setup_server(inet_port, socket_path, &socket_fd);

    pthread_t connection_manager;
    pthread_t ping_manager;
    pthread_t game_manager;

    pthread_create(&connection_manager, NULL, connection_manager_f, NULL);
    pthread_create(&ping_manager, NULL, ping_manager_f, NULL);
    pthread_create(&game_manager, NULL, game_manager_f, NULL);

    pthread_join(connection_manager, NULL);
    pthread_join(ping_manager, NULL);
    pthread_join(game_manager, NULL);

    return 0;
}