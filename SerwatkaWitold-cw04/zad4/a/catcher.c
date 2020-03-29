#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

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

//=================SEND====================//

void send_kill(int pid, int count){
    for(int i = 0; i < count; i++){
        kill(pid, SIGUSR1);
        sleep(0.5);
    }

    kill(pid, SIGUSR2);
}

void send_queue(int pid, int count){
    union sigval u;
    u.sival_int = 0;

    for(int i = 0; i < count; i++){
        sigqueue(pid, SIGUSR1, u);
        sleep(0.5);
    }

    sigqueue(pid, SIGUSR2, u);
}

//==============RECEIVE===================//

void handler_kill(int sig_num, siginfo_t* info, void* context){
    static int counter = 0;

    if(sig_num == SIGUSR1){
        counter++;
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
        sigsuspend(&mask);
    }
    else if(sig_num == SIGUSR2){
        printf("Received %d SIGUSR1 signals in total.\n", counter);
        
        send_kill(info->si_pid, counter);
    }
    else{
        printf("Received signal number %d.\n", sig_num);
    }
}

void receive_kill(){
    struct sigaction act;

    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGUSR1);
    sigdelset(&act.sa_mask, SIGUSR2);

    act.sa_sigaction = handler_kill;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGUSR1, &act, NULL) == -1){
        printf("Could not set action for SIGUSR1.\n");
        error();
    }
    if(sigaction(SIGUSR2, &act, NULL) == -1){
        printf("Could not set action for SIGUSR2.\n");
        error();
    }

    pause();
}

void handler_queue(int sig_num, siginfo_t* info, void* context){
    static int counter = 0;

    if(sig_num == SIGUSR1){
        union sigval u;
        u.sival_int = ++counter;
        sigqueue(info->si_pid, SIGUSR1, u);

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
        sigsuspend(&mask);
    }
    else if(sig_num == SIGUSR2){
        printf("Received %d SIGUSR1 signals in total.\n", counter);
        
        send_queue(info->si_pid, counter);
    }
    else{
        printf("Received signal number %d.\n", sig_num);
    }
}

void receive_queue(){
    struct sigaction act;

    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGUSR1);
    sigdelset(&act.sa_mask, SIGUSR2);

    act.sa_sigaction = handler_queue;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGUSR1, &act, NULL) == -1){
        printf("Could not set action for SIGUSR1.\n");
        error();
    }
    if(sigaction(SIGUSR2, &act, NULL) == -1){
        printf("Could not set action for SIGUSR2.\n");
        error();
    }

    pause();
}

//=================MAIN====================//

int main(int argc, char** argv){
    if(argc != 2 || is_number(argv[1]) == 1){
        error();
    }

    pid_t pid = getpid();
    printf("Catcher pid: %d\n", pid);

    char* mode = argv[1];

    if(strcmp(mode, "kill") == 0){
        receive_kill();
    }
    else if(strcmp(mode, "sigqueue") == 0){
        receive_queue();   
    }

    return 0;
}