#ifndef settings
#define settings

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PROJECT 'a'
#define NSEMS 20

void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

#endif