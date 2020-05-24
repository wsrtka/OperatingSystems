#include "common.h"


//global args


void open_server();

void close_server();


void* ping_manager_f(void* args);


int main(int argc, char* argv[]){

    atexit(close_server);
    signal(SIGINT, close_server);


    if(argc != 3){
        error("Invalid number of arguments.");
    }

    //get command line args


    open_server();


    pthread_t ping_manager;

    pthread_create(ping_manager, NULL, ping_manager_f, NULL);


    close_server();


    return 0;

}