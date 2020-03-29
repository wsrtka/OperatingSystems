#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

void error(){
    printf("Program usage: ./exec <option>\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    if(argc != 2){
        error();
    }

    sigset_t check;

    if(strcmp(argv[1], "ignore") == 0){
        printf("Signal is being ignored in exec\n");
    }
    else if(strcmp(argv[1], "mask") == 0){
        printf("Signal is being masked in exec\n");
    }
    else if(strcmp(argv[1], "handler") == 0){
        printf("Signal is being handled in exec\n");
    }
    else if(strcmp(argv[1], "pending") == 0){
        sigpending(&check);
        if(sigismember(&check, SIGUSR1) == 1){
            printf("SIGUSR1 is pending in parent\n");
        }
        else{
            printf("SIGUSR1 is not pending in parent");
        }
    }

    return 0;
}