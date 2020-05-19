#include "common.h"

char* name;
int connection_type;
char* server_address;
in_port_t server_port;

int socket_fd;
char* server_name;

char game_symbol;

//===========CLIENT FUNCTIONS=================//

void open_client(int* fd, char* path, char* name){

    struct sockaddr_un addr;
    strcpy(addr.sun_path, path);
    addr.sun_family = AF_UNIX;

    if((*fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        error("Could not create socket.");
    }   

    if(connect(*fd, (struct sockaddr*) &addr, sizeof(addr)) != 0){
        error("Could not connect to server.");
    }

    struct pollfd* listener;
    listener->fd = *fd;
    listener->events = POLLIN;

    poll(listener, 1, -1);
    char* handshake;

    if(read(*fd, handshake, MSG_LEN) < 1){
        error("Could not receive handshake from server.");
    }

    if(strcmp(handshake, "Client registered.") == 0){
        printf("Server handshake verified.\n");
    }

    write(*fd, name, sizeof(name));

}

void close_client(){
    
    if(shutdown(socket_fd, SHUT_RDWR) == -1){
        printf("Unable to shut down socket.");
    }

    if(close(socket_fd) == -1){
        printf("Could not close socket.");
    }

}

//=========GAME FUNCTIONS============//

void visualise_board(char* board){

    if(strlen(board) != 9){
        error("Invalid board size.");
    }

    for(int i = 0; i < 9; i++){

        printf(" %s ", board[i]);
        
        if(i % 3 != 2){
            printf("|");
        }
        else{
            printf("\n");

            if(i != 9){
                printf("---+---+---\n");;
            }
        }

    }

}

void play(char* game_state){

    visualise_board(game_state);

    printf("It's your turn now, enter the choosen field number.\n");
    
    char* decision;
    scanf("%s", &decision);

    game_state[atoi(decision) - 1] = game_symbol;

}

/*
 1 | 2 | 3
---+---+---
 4 | 5 | 6 
---+---+---
 7 | 8 | 9 
*/

//===========MSG HANDLER=============//

void handle_connection(){

    struct pollfd* listener;
    listener->fd = socket_fd;
    listener->events = POLLIN;

    char* buffer;

    while(1){

        poll(listener, 1, -1);

        if(read(socket_fd, buffer, MSG_LEN) == -1){
            error("Could not read message from server.");
        }

        if(strcmp(buffer, "ping") == 0){
            write(socket_fd, "pong", 4);
        }
        else if(strcmp(buffer, "Client with this name already exists.") == 0){
            error(buffer);
        }
        else{
            play(buffer);
            
            write(socket_fd, buffer, sizeof(buffer));
        }

    }

    return;
}

int main(int argc, char* argv[]){

    atexit(close_client);
    signal(SIGINT, close_client);

    if(strcmp(argv[2], "unix") == 0){
        
        if(argc != 4){
            error("Invalid number of arguments.");
        }

        name = argv[1];
        connection_type = AF_UNIX;
        server_address = argv[3];

    }
    else if(strcmp(argv[2], "inet") == 0){

        if(argc != 5){
            error("Invalid number of arguments.");
        }

        name = argv[1];
        connection_type = AF_INET;
        server_address = argv[3];
        server_port = (in_port_t) atoi(argv[4]);

    }

    open_client(&socket_fd, server_address, name);

    handle_connection();

    close_client();

    return 0;

}