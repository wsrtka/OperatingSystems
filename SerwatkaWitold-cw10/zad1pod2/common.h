#ifndef common
#define common

//standard libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//libs needed for socket operations
#include <sys/socket.h>

//libs for threading
#include <pthread.h>

void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

#endif