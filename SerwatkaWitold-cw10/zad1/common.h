#ifndef common
#define common

#define MAX_CLIENTS 16
#define NAME_LEN 64
#define MSG_LEN 256

#define PING_INTERVAL 60
#define PING_TIMEOUT 30

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include <netinet/in.h>

#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

typedef struct client{
    int socket_fd;
    int registered;
    int active;
    int game_partner;
    char* name;
} client;

void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

#endif