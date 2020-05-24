#include "common.h"

void close_server();

int main(int argc, char* argv[]){

    atexit(close_server);
    signal(SIGINT, close_server);

    close_server();

    return 0;

}