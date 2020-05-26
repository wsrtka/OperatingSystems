#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define _XOPEN_SOURCE 700

volatile int sigtstop = 0;

void sigint_handler(){
    printf("Odebrano sygnał SIGINT\n");
    exit(EXIT_SUCCESS);
}

void sigtstop_handler(){
    if(sigtstop == 0){
        printf("Oczeskuję na CTRL+Z - kontynuacja - albo CRTL+C - zakończenie programu.\n");
        sigtstop = 1;
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGINT);
        sigdelset(&mask, SIGTSTP);
        sigsuspend(&mask);
    }
    else{
        sigtstop = 0;
    }
}

int main(){
    signal(SIGINT, sigint_handler);

    struct sigaction act;
    act.sa_handler = sigtstop_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);    


    while(1){
        if(sigtstop == 0){
            system("ls -alR");
        }
        sleep(2);
    }

    return 0;
}