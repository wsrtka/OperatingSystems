#ifndef header
#define header

#include <stdlib.h>

#define MSG_SIZE 256
#define PATHLENGTH 128
#define MAXCLIENTS 10
#define STOP  0
#define DISCONNECT 1
#define INIT 2
#define LIST 3
#define CONNECT 4
#define PROJECT 'c'

struct msgbuf
{
    long mtype;
    char mtext[MSG_SIZE];
    key_t obj_key;
};


void error(char* msg){
    printf(msg);
    exit(EXIT_FAILURE);
}

#endif