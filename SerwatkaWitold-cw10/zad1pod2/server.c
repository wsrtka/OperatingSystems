
#include "common.h"


int socket_fd;
Client* clients;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void open_server(char* path){

    clients = (Client*) calloc(MAX_CLIENTS, sizeof(Client));

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].name = '\0';
        clients[i].playing = 0;
        clients[i].registered = 0;
        clients[i].socket_fd = -1;
    }

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

    if(epoll_fd != -1){
        if(close(epoll_fd) == -1){
            printf("Could not close epoll.\n %s\n", strerror(errno));
        }
        else{
            printf("Epoll closed.\n");
        }
    }

    printf("Server shutdown.\n");

}


void* connection_manager_f(void* args){

    int i, new_fd;

    while(1){

        new_fd = accept(socket_fd, NULL, NULL);

        printf("New connection request.\n");

        pthread_mutex_lock(&lock);

        for(i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].socket_fd == -1){

                printf("Found free client slot.\n");
                clients[i].socket_fd = new_fd;
                break;

            }

        }

        if(i == MAX_CLIENTS){

            printf("No slot found.\n");

            //return message to client.        
    
        }
        else{

            //send success message to client and get his name

        }

        pthread_mutex_unlock(&lock);

    }

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


    // pthread_t ping_manager;
    pthread_t connection_manager;

    pthread_create(&connection_manager, NULL, connection_manager_f, NULL);
    // pthread_create(ping_manager, NULL, ping_manager_f, NULL);

    pthread_join(connection_manager, NULL);


    close_server();


    return 0;

}