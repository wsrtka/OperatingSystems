#ifndef common
#define common

//standard libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

//libs needed for socket operations
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

//libs for threading
#include <pthread.h>

//constants definitions
#define UNIX_PATH_MAX 108
#define LOSER 0
#define WINNER 1

//settings
#define MAX_CLIENTS 16
#define MSG_SIZE 256
#define PING_INTERVAL 60
#define PING_TIMEOUT 30
#define SERVER_WAIT 5000

//message types
#define REJECT 0
#define ACCEPT 1
#define PING 2
#define SYMBOLSET 3

//struct for client info on server
typedef struct Client
{
    int socket_fd;
    int registered;
    int playing;
    char* name;
} Client;



void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

#endif