
#include "common.h"


int socket_fd;


void open_client(){

    if((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        error("Could not create socket.");
    }
    printf("Socket successfully created.\n");

    

}

void close_client();


int main(int argc, char* argv[]){

    atexit(close_client);
    signal(SIGINT, close_client);


    if(argc == 4){

        char* name = argv[1];
        char* connection_type = argv[2];
        char* server_path = argv[3];
        
    }
    else if(argc == 5){
        //define settings for ITNET connection
    }
    else{
        error("Invalid number of arguments.");
    }


    open_client();


    close_client();


    return 0;

}