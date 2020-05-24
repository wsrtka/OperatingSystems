#ifndef common
#define common

//standard libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

//libs needed for socket operations
#include <sys/socket.h>
#include <sys/un.h>

//libs for threading
#include <pthread.h>

//constants definitions
#define UNIX_PATH_MAX 108

//settings
#define MAX_CLIENTS 16

void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

#endif