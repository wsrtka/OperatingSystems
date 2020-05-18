#ifndef common
#define common

#define MAX_CLIENTS 16

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include <netinet/in.h>

#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct client{
    int socket_fd;
    int registered;
    int active;
    char* name;
} client;

//helper functions
void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}



#endif