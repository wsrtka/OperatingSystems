#include "common.h"

in_port_t inet_port;
char* socket_path;
int socket_fd;

client clients[MAX_CLIENTS];

pthread_mutex_t mut_clients = PTHREAD_MUTEX_INITIALIZER;

int non_active_players = 0;

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

//===========HELPER FUNCITONS==============//

void close_client(int id){
    clients[id].registered= 0;
    clients[id].name = "\0";
    close(clients[id].socket_fd);
    clients[id].socket_fd = -1;

    non_active_players--;
}

void activate_client(int id){
    clients[i].registered = 1;
}

void process_msg(int id){
    char* buffer;

    int fd = clients[i].socket_fd;

    if(read(fd, buffer, MSG_SIZE) == -1){
        error("Could not read incoming client message.");
    }

    if(strcmp(buffer, "pong") == 0){
        activate_client(id)
    }
    else{
        //kod na gre w kolko i krzyzyk
    }
}

void start_game(int id){

    for(int i = 0; i < MAX_CLIENTS; i++){

        if(clients[i].registered == 1 && clients[i].active == 0 && i != id){

            struct pollfd* players;
            players[0].fd = clients[i].socket_fd;
            players[0].events = POLLIN;
            players[1].fd = clients[id].socket_fd;
            players[1].events = POLLIN;

            write(clients[i].socket_fd, "Game partner found, are you ready?", 34);
            write(clients[id].socket_fd, "Game partner found, are you ready?", 34);

            int res = poll(players, 2, 15);

            if(res != 2){
                continue;
            }
            
            char* buffer;
            if(read(clients[i].socket_fd, buffer, MSG_LEN) < 1){
                continue;
            }

            if(strcmp(buffer, "y") != 0){
                continue;
            }

            if(read(clients[id].socket_fd, buffer, MSG_LEN) < 1){
                continue;
            }

            if(strcmp(buffer, "y") != 0){
                continue;
            }

            if(rand() % 2 == 0){
                write(clients[i].socket_fd, "x", 1);
                write(clients[id].socket_fd, "o", 1);

                return;
            }
            else{
                write(clients[id].socket_fd, "x", 1);
                write(clients[i].socket_fd, "o", 1);

                return;
            }

        }

    }

    write(clients[id].socket_fd, "Could not find game partner.", 29);

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

                write(new_client_fd, "Client request received.", 25);

                struct pollfd* listener;
                listener->fd = new_client_fd;
                listener->events = POLLIN;

                poll(listener, 1, -1);

                char* new_name;
                read(new_client_fd, new_name, NAME_LEN);

                int j;
                for(j = 0; j < MAX_CLIENTS; j++){
                    if(strcmp(clients[j].name, new_name) == 0){
                        write(new_client_fd, "Client with this name already exists.", 38);
                        break;
                    }
                }

                if(j == MAX_CLIENTS){
                    clients[i].socket_fd = new_client_fd;
                    clients[i].registered = 1;
                }

                non_active_players++;

                start_game(i);
                
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

        sleep(PING_INTERVAL);

        pthread_mutex_lock(&mut_clients);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].registered == 1){
                clients[i].active = 0;

                write(clients[i].socket_fd, "ping", 4);
            }

        }

        pthread_mutex_unlock(&mut_clients);

        sleep(PING_INTERVAL);

        pthread_mutex_unlock(&mut_clients);

        for(int i = 0; i < MAX_CLIENTS; i++){

            if(clients[i].registered == 0 && clients[i].socket_fd != -1){
                close_client(i);
            }

        }

        pthread_mutex_unlock(&mut_clients);

    }

    return 0;
}

void* client_manager_f(void* args){

    struct pollfd* fds = calloc(MAX_CLIENTS, sizeof(struct pollfd));

    for(int i = 0; i < MAX_CLIENTS; i++){
        fds[i].events = POLLIN;
    }

    int check;

    while(1){

        pthread_mutex_lock(&mut_clients);

        for(int i = 0; i < MAX_CLIENTS; i++){
            fds[i].fd = clients[i].socket_fd;
        }

        pthread_mutex_unlock(&mut_clients);

        if((check = poll(fds, MAX_CLIENTS, -1)) == -1){
            error("Client manager fatal error.");
        }

        for(int i = 0; i < MAX_CLIENTS && check > 0; i++){

            if(fds[i].revents == POLLIN){       //możliwe, że trzeba użyć &
                pthread_mutex_lock(&mut_clients);

                process_msg(i);

                pthread_mutex_unlock(&mut_clients);

                check--;
            }
            else if(fds[i].revents == POLLHUP){
                pthread_mutex_lock(&mut_clients);

                close_client(i);

                pthread_mutex_unlock(&mut_clients);

                check--;
            }

        }
    }

    return 0;
}

int main(int argc, char* argv[]){
    
    atexit(close_server);
    signal(SIGINT, close_server);

    srand(time(NULL));

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
    pthread_t client_manager;

    pthread_create(&connection_manager, NULL, connection_manager_f, NULL);
    pthread_create(&ping_manager, NULL, ping_manager_f, NULL);
    pthread_create(&client_manager, NULL, client_manager_f, NULL);

    pthread_join(connection_manager, NULL);
    pthread_join(ping_manager, NULL);
    pthread_join(client_manager, NULL);

    close_server();

    return 0;
}