
#include "common.h"


int socket_fd;
int epoll_fd = -1;
char* game_symbol;


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
    
    if(atoi(msg) == REJECT){
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

    if(atoi(msg) == REJECT){
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

    if(epoll_fd != -1){

        if(close(epoll_fd) == -1){
            printf("Could not close epoll_fd.\n");
        }
        else{
            printf("Epoll_fd closed.\n");
        }

    }

    printf("Client shutdown.\n");

    exit(EXIT_SUCCESS);

}


void visualise_board(char* game_state){

    for(int i = 0; i < 9; i++){

        printf(" %d ", atoi(game_state[i]));
        
        if(i % 3 != 2){
            printf("|");
        }
        else{
            printf("\n");

            if(i != 8){
                printf("---+---+---\n");
            }
        }

    }

}

int check_choice(char* game_state, char* choice){

    if(game_state[atoi(choice) - 1] == 'x' 
    || game_state[atoi(choice) - 1] == 'o' 
    || atoi(choice) < 1 
    || atoi(choice) > 9){
        return 0;
    }

    return 1;

}

int is_final(char* game_state, char* symbol){

    int streak1, streak2, streak3;

    for(int i = 0; i < 3; i++){

        streak1 = 0;
        streak2 = 0;
        streak3 = 0;

        for(int j = 0; j < 3; j++){

            if(game_state[i + j*3] == symbol[0]){
                streak1++;
            }

            if(game_state[i*3 + j] == symbol[0]){
                streak2++;
            }

            if(i == 0 && game_state[j + j*3] == symbol[0]){
                streak3++;
            }

            if(i == 1 && game_state[(2 - j) + j*3] == symbol[0]){
                streak3++;
            }

        }

        if(streak1 == 3 || streak2 == 3 || streak3 == 3){
            return 1;
        }

    }

    return 0;

}

void play(char* game_state){

    if(is_final(game_state, strcmp(game_symbol, "x") == 0 ? "o" : "x") == 1){
        end_game(LOSER);
    }

    int is_legal = 0;
    char* choice = calloc(MSG_SIZE, sizeof(char));

    do{

        visualise_board(game_state);

        printf("Choose a free spot.\n");
        scanf("%s", choice);

        is_legal = check_choice(game_state, choice);

    }while(!is_legal);

    game_state[atoi(choice) - 1] = game_symbol[0];

    if(write(socket_fd, game_state, MSG_SIZE) < 1){
        error("Could not send server response.");
    }

    if(is_final(game_state, game_symbol) == 1){
        end_game(WINNTER);
    }

    free(choice);

}

void end_game(int is_winner){

    if(is_winner == WINNTER){
        printf("Congratulations! You won the game.\n Client will close automatically in 5 seconds.\n");
    }
    else if(is_winner == LOSER){
        printf("You lost!\n Client will close automatically in 5 seconds.\n");
    }

    sleep(5);
    raise(SIGINT);

}


void* server_listener_f(void* args){

    char* msg = (char*) calloc(MSG_SIZE, sizeof(char));

    if((epoll_fd = epoll_create1(0)) == -1){
        error("Could not create server listener.");
    }

    struct epoll_event event;
    event.events = EPOLLIN;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1){
        error("Could not set up server listener.");
    }


    while(1){

        epoll_wait(epoll_fd, &event, 1, -1);

        if(read(socket_fd, msg, MSG_SIZE) < 1){
            error("Could not read message from server.\n");
        }

        if(atoi(msg) == PING){

            printf("debug: received ping msg from server.\n");

            if(write(socket_fd, msg, MSG_SIZE) < 1){
                error("Could not send server response.");
            }

        }
        else if(atoi(msg) == REJECT){

            raise(SIGINT);

        }
        else if(atoi(msg) == SYMBOLSET){

            game_symbol = "o";

        }
        else if(strlen(msg) == 9){

            play(msg);

        }
        else{

            error("Invalid server message.\n");

        }

    }

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


    pthread_t server_listener;

    pthread_create(&server_listener, NULL, server_listener_f, NULL);

    pthread_join(server_listener, NULL);


    close_client();


    return 0;

}