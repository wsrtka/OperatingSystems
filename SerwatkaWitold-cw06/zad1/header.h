#ifndef header
#define header

#include <stdlib.h>

#define MSG_SIZE 256
#define PATHLENGTH 128
#define MAXCLIENTS 32
#define STOP  0
#define DISCONNECT 1
#define LIST 2
#define CONNECT 3
#define INIT 4
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