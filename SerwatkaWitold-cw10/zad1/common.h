#ifndef common
#define common

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

//helper functions
void error(char* msg){
    printf("%s\n", msg);
    printf("%s", strerror(errno));
    exit(EXIT_FAILURE);
}



#endif