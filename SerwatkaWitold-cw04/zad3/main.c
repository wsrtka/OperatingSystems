#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void division_handler(int sig, siginfo_t* info, void* context){
    if(info->si_code == FPE_INTDIV){
        printf("Division by zero exception: %d\n", FPE_INTDIV);
    }

    exit(1);
}

void segmentation_handler(int sig, siginfo_t* info, void* context){
    printf("Segmentation fault exception occured on address %d\n", info->si_addr);
    exit(1);
}

void child_handler(int sig, siginfo_t* info, void* context){
    printf("Child has finished execution returning %d\n", info->si_status);
}

int main(){
    struct sigaction div_act;
    div_act.sa_sigaction = division_handler;
    div_act.sa_flags = SA_SIGINFO;
    sigemptyset(&div_act.sa_mask);
    sigaction(SIGFPE, &div_act, NULL);

    struct sigaction seg_act;
    seg_act.sa_sigaction = segmentation_handler;
    seg_act.sa_flags = SA_SIGINFO;
    sigemptyset(&seg_act.sa_mask);
    sigaction(SIGFPE, &seg_act, NULL);

    struct sigaction child_act;
    child_act.sa_sigaction = child_handler;
    child_act.sa_flags = SA_SIGINFO;
    sigemptyset(&child_act.sa_mask);
    sigaction(SIGFPE, &child_act, NULL);
}
