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
    printf("Socket created.\n");

    if(connect(*fd, (struct sockaddr*) &addr, sizeof(addr)) != 0){
        error("Could not connect to server.");
    }
    printf("Connected to server.\n");

    struct pollfd* listener = calloc(1, sizeof(struct pollfd));
    listener->fd = *fd;
    listener->events = POLLIN;

    poll(listener, 1, -1);
    char handshake[MSG_LEN];

    if(read(*fd, handshake, MSG_LEN) < 1){
        error("Could not receive handshake from server.");
    }
    printf("Received handshake from server.\n");

    if(strcmp(handshake, "Client request received.") == 0){
        printf("Server handshake verified.\n");
    }

    write(*fd, name, sizeof(name));

}

void close_client(){
    
    if(shutdown(socket_fd, SHUT_RDWR) == -1){
        printf("Unable to shut down socket.\n");
        printf("%s\n", strerror(errno));
    }
    else{
        printf("Socket shutdown.\n");
    }

    if(close(socket_fd) == -1){
        printf("Could not close socket.\n");
        printf("%s\n", strerror(errno));
    }
    else{
        printf("Socket closed.\n");
    }

}

//=========GAME FUNCTIONS============//

void visualise_board(char* board){

    if(strlen(board) != 9){
        error("Invalid board size.");
    }

    for(int i = 0; i < 9; i++){

        printf(" %d ", board[i]);
        
        if(i % 3 != 2){
            printf("|");
        }
        else{
            printf("\n");

            if(i != 8){
                printf("---+---+---\n");;
            }
        }

    }

}

void play(char* game_state){

    visualise_board(game_state);

    printf("It's your turn now, enter the choosen field number.\n");
    
    char decision[MSG_LEN];
    scanf("%s", decision);

    game_state[atoi(decision) - 1] = game_symbol;

}

//===========MSG HANDLER=============//

void handle_connection(){

    struct pollfd listener[1];
    listener[0].fd = socket_fd;
    listener[0].events = POLLIN;

    char buffer[MSG_LEN];

    while(1){

        buffer[0] = '\0';

        poll(listener, 1, -1);

        if(read(socket_fd, buffer, sizeof(buffer)) == -1){
            printf("%d", listener[0].revents & POLLIN);
            error("Could not read message from server.");
        }

        if(strcmp(buffer, "") != 0){
            printf("received message: %s\n", buffer);
        }

        if(strcmp(buffer, "ping") == 0){
            write(socket_fd, "pong", 4);
        }
        else if(
            strcmp(buffer, "Client with this name already exists.") == 0
            || strcmp(buffer, "Could not find game partner.") == 0
        ){
            // error(buffer);
            printf("%s\n", buffer);
        }
        else if(strcmp(buffer, "Game partner found, are you ready?") == 0){
            printf("%s [y/n]\n", buffer);
            
            char answer[MSG_LEN];
            scanf("%s", answer);

            write(socket_fd, answer, strlen(answer));
            
            answer[0] = '\0';
            buffer[0] = '\0';

            poll(listener, 1, -1);

            read(socket_fd, buffer, MSG_LEN);

            if(strcmp(buffer, "x") == 0){
                game_symbol = 'x';
                printf("You begin! Your symbol is: x\n");
                play("123456789");
            }
            else{
                game_symbol = 'o';
                printf("Waiting for opponent to finish turn. Your symbol is: o\n");
            }
        }
        else if (strlen(buffer) == 9){
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