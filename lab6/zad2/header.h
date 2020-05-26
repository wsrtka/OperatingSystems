#ifndef header
#define header

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#define MSG_SIZE 16
#define PATHLENGTH 128
#define MAXCLIENTS 10
#define STOP  4
#define DISCONNECT 3
#define INIT 2
#define LIST 1
#define CONNECT 0
#define PROJECT 'b'



void error(char* msg){
    printf("%s", msg);
    raise(SIGINT);
    exit(EXIT_FAILURE);
}

#endif