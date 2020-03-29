#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

//=============HELPER FUNCTIONS=============//

void error(){
    printf("Program usage: ./sender <catcher pid> <signals nr> <mode>\n");
    exit(EXIT_FAILURE);
}

int is_number(char* str){
    for(int i = 0; i < strlen(str); i++){
        if(isdigit(str[i]) == 0){
            return 0;
        }
    }
    return 1;
}

//=================KILL====================//

void send_kill(int pid, int count){
    
}

//==============SIGQUEUE===================//

//================SIGRT====================//

//=================MAIN====================//

int main(int argc, char** argv){
    if(argc != 4 || is_number(argv[1]) == 0 || is_number(argv[2]) == 0 || is_number(argv[3]) == 1){
        error();
    }

    int catcher_pid = atoi(argv[1]);
    int signals_count = atoi(argv[2]);
    char* mode = argv[3];

    if(strcmp(mode, "kill") == 0){

    }
    else if(strcmp(mode, "sigqueue") == 0){

    }
    else if(strcmp(mode, "sigrt") == 0){
        
    }
    else{
        error();
    }

    return 0;
}