
#include "common.h"


int socket_fd = -1;
int epoll_fd = -1;
Client* clients;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void open_server(char* path){

    clients = (Client*) calloc(MAX_CLIENTS, sizeof(Client));

    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].name = (char*) calloc(MSG_SIZE, sizeof(char));
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
// TODO: close each individual socket and send signal to client
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

}


void* connection_manager_f(void* args){

    int i, j, new_fd;
    char* msg = (char*) calloc(MSG_SIZE, sizeof(char));

    while(1){

        new_fd = accept(socket_fd, NULL, NULL);

        printf("New connection request received.\n");

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

            snprintf(msg, MSG_SIZE, "%d", REJECT);

            if(write(new_fd, msg, sizeof(msg)) < 1){
                printf("Could not send response to client.\n %s\n", strerror(errno));
            }      
    
        }
        else{

            snprintf(msg, MSG_SIZE, "%d", ACCEPT);
        

            if(write(new_fd, msg, sizeof(msg)) < 1){
                printf("Could not send response to client.\n %s\n", strerror(errno));
            }

            free(msg);
            char* msg = (char*) calloc(MSG_SIZE, sizeof(char));

            if(read(new_fd, msg, MSG_SIZE) < 1){
                printf("Could not read client name.\n %s\n", strerror(errno));
            }

            
            for(j = 0; j < MAX_CLIENTS; j++){

                if(strcmp(clients[j].name, msg) == 0){

                    printf("Client with this name already exists.\n");
                    snprintf(msg, MSG_SIZE, "%d", REJECT);
                    if(write(new_fd, msg, sizeof(msg)) < 1){
                        printf("Could not send response to client.\n");
                    }
                    break;

                }

            }


            if(j == MAX_CLIENTS){


                strcpy(clients[i].name, msg);
                clients[i].registered = 1;

                snprintf(msg, MSG_SIZE, "%d", ACCEPT);

                if(write(new_fd, msg, sizeof(msg)) < 1){
                    printf("Could not send response to client.\n %s\n", strerror(errno));
                }


            }
            else{

                snprintf(msg, MSG_SIZE, "%d", REJECT);

                if(write(new_fd, msg, sizeof(msg)) < 1){
                    printf("Could not send response to client.\n %s\n", strerror(errno));
                }

            }

        }


        pthread_mutex_unlock(&lock);

    }

}

void* ping_manager_f(void* args){

    char* msg = calloc(MSG_SIZE, sizeof(char));

    snprintf(msg, MSG_SIZE, "%d", PING);

    while(1){

        sleep(PING_INTERVAL);

        pthread_mutex_lock(&lock);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].socket_fd != -1){
                printf("debug: sent ping to client no. %d\n", i);

                if(write(clients[i].socket_fd, msg, MSG_SIZE) < 1){
                    printf("Could not ping client with id %d\n", i);
                }

                clients[i].registered = 0;

            }

        }

        pthread_mutex_unlock(&lock);

        sleep(PING_TIMEOUT);

        pthread_mutex_lock(&lock);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].registered == 0 && clients[i].socket_fd != -1){

                strcpy(clients[i].name, "");
                clients[i].playing = 0;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clients[i].socket_fd, NULL);
                clients[i].socket_fd = -1;

                printf("debug: disconnected client no. %d\n", i);

            }

        }

        pthread_mutex_unlock(&lock);

    }

}

void* comm_manager_f(void* args){

    int count;
    char* msg = (char*) calloc(MAX_CLIENTS, sizeof(char));

    epoll_fd = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN;

    struct epoll_event* events = (struct epoll_event*) calloc(MAX_CLIENTS, sizeof(struct epoll_event));


    while(1){

        pthread_mutex_lock(&lock);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].socket_fd != -1){
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clients[i].socket_fd, &event);
            }

            events[i].events = 0;

        }


        pthread_mutex_unlock(&lock);

        printf("debug: %d\n", 0);
        count = epoll_wait(epoll_fd, events, MAX_CLIENTS, SERVER_WAIT);
        printf("debug: %d\n", count);

        pthread_mutex_lock(&lock);

        for(int i = 0; i < MAX_CLIENTS && count > 0; i++){

            if(!(events[i].events ^ EPOLLIN)){

                if(read(clients[i].socket_fd, msg, MSG_SIZE) < 1){
                    printf("Could not read message from client no. %d.\n", i);
                    continue;
                }

                if(atoi(msg) == PING){

                    printf("Received pong message from client no %d.\n", i);

                    clients[i].registered = 1;

                }
                else if(strlen(msg) == 9){

                    if(write(clients[clients[i].playing].socket_fd, msg, MSG_SIZE) < 1){
                        
                        printf("Could not send game state from player no. %d to player no. %d.\n", i, clients[i].playing);
                        
                        snprintf(msg, MSG_SIZE, "%d", REJECT);

                        write(clients[i].socket_fd, msg, MSG_SIZE);
                        write(clients[clients[i].playing].socket_fd, msg, MSG_SIZE);
                        
                    }

                }
                else{

                    printf("Received invalid message from client no. %d.\n", i);

                }

            }

        }


        pthread_mutex_unlock(&lock);

    }

}


int main(int argc, char* argv[]){

    atexit(close_server);
    signal(SIGINT, close_server);


    if(argc != 3){
        error("Invalid number of arguments.");
    }

    char* socket_path = argv[2];


    open_server(socket_path);


    pthread_t ping_manager;
    pthread_t connection_manager;
    pthread_t comm_manager;

    pthread_create(&connection_manager, NULL, connection_manager_f, NULL);
    pthread_create(&ping_manager, NULL, ping_manager_f, NULL);
    pthread_create(&comm_manager, NULL, comm_manager_f, NULL);

    pthread_join(connection_manager, NULL);
    pthread_join(ping_manager, NULL);
    pthread_join(comm_manager, NULL);


    close_server();


    return 0;

}