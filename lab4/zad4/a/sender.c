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
        return;
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
        if(info->si_value.sival_int == 0){
            counter++;
            sigset_t mask;
            sigfillset(&mask);
            sigdelset(&mask, SIGUSR1);
            sigdelset(&mask, SIGUSR2);
            sigsuspend(&mask);
        }
        else{
            printf("Catcher has received signal number %d\n", info->si_value.sival_int);
        }
    }
    else if(sig_num == SIGUSR2){
        printf("Received %d SIGUSR1 signals in total.\n", counter);
        return;
    }
    else{
        printf("Received signal number %d.\n", sig_num);
        return;
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

void handler_sigrt(int sig_num, siginfo_t* info, void* context){
    static int counter = 0;

    if(sig_num == SIGRTMIN){
        counter++;
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGRTMAX);
        sigsuspend(&mask);
    }
    else if(sig_num == SIGRTMAX){
        printf("Received %d SIGRTMIN signals in total.\n", counter);
        return;
    }
    else{
        printf("Received signal number %d.\n", sig_num);
    }
}

void receive_sigrt(){
    struct sigaction act;

    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGRTMIN);
    sigdelset(&act.sa_mask, SIGRTMAX);

    act.sa_sigaction = handler_sigrt;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGRTMIN, &act, NULL) == -1){
        printf("Could not set action for SIGUSR1.\n");
        error();
    }
    if(sigaction(SIGRTMAX, &act, NULL) == -1){
        printf("Could not set action for SIGUSR2.\n");
        error();
    }

    pause();
}

//=================SEND====================//

void send_kill(int pid, int count){
    for(int i = 0; i < count; i++){
        kill(pid, SIGUSR1);
        sleep(0.5);
    }

    kill(pid, SIGUSR2);

    receive_kill();
}

void send_queue(int pid, int count){
    struct sigaction act;

    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGUSR1);

    act.sa_sigaction = handler_queue;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGUSR1, &act, NULL) == -1){
        printf("Could not set action for SIGUSR1.\n");
        error();
    }

    union sigval u;
    u.sival_int = 0;

    for(int i = 0; i < count; i++){
        sigqueue(pid, SIGUSR1, u);
        sleep(0.5);
    }

    sigqueue(pid, SIGUSR2, u);

    receive_queue();
}

void send_sigrt(int pid, int count){
    for(int i = 0; i < count; i++){
        kill(pid, SIGRTMIN);
        sleep(1);
    }

    kill(pid, SIGRTMAX);

    receive_sigrt();
}

//=================MAIN====================//

int main(int argc, char** argv){
    if(argc != 4 || is_number(argv[1]) == 0 || is_number(argv[2]) == 0 || is_number(argv[3]) == 1){
        error();
    }

    int catcher_pid = atoi(argv[1]);
    int signals_count = atoi(argv[2]);
    char* mode = argv[3];

    if(strcmp(mode, "kill") == 0){
        send_kill(catcher_pid, signals_count);
    }
    else if(strcmp(mode, "sigqueue") == 0){
        send_queue(catcher_pid, signals_count);
    }
    else if(strcmp(mode, "sigrt") == 0){
        send_sigrt(catcher_pid, signals_count);
    }
    else{
        error();
    }

    printf("Should have received %d signals.\n", signals_count);

    return 0;
}