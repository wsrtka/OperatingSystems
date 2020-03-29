#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void handler(int sig_num, siginfo_t* info, void* context){
    printf("Proces zużył %ld sekund czasu użytkownika.\n", info->si_utime/CLOCKS_PER_SEC);
    printf("Proces zużył %ld sekund czasu systemowego.\n", info->si_stime/CLOCKS_PER_SEC);
    printf("Prawdziwy UID wysyłającego procesu: %d\n", info->si_uid);
}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);

    act.sa_mask = mask;
    act.sa_sigaction = handler;

    if(sigaction(SIGTSTP, &act, NULL) == -1){
        printf("Nie udało się przypisać handlera\n");
        exit(EXIT_FAILURE);
    }

    sleep(2);

    raise(SIGTSTP);

    return 0;
}