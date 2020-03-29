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
    

    return 0;
}