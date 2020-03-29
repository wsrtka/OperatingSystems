#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd;

void error(){
    printf("Program usage: ./main <option>\n");
    exit(EXIT_FAILURE);
}

void handler(){
    char* msg = "Otrzymano sygnał SIGUSR1\n";
    printf("%s", msg);
    if(write(fd, msg, strlen(msg)) != strlen(msg)){
        printf("Nie udało się zapisać wyniku do pliku\n");
        error();
    }
}

void sig_handler(){
    char* msg = "Podstawowy handler otrzymał sygnał SIGUSR1\n";
    printf("%s", msg);
    if(write(fd, msg, strlen(msg)) != strlen(msg)){
        printf("nie udało się zapisać wyniku do pliku\n");
        error();
    }
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Nieprawidłowa liczba argumentów\n");
        error();
    }

    fd = open("raport2.txt", O_WRONLY | O_APPEND);
    if(fd == -1){
        printf("Nie udało się otworzyć pliku 'raport2.txt'\n");
    }

    signal(SIGUSR1, sig_handler);

    if(strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }
    else if(strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);
    }
    else if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1){
            printf("Nie udało się ustawić maski sygnałów\n");
            error();
        }
    }
    else{
        error();
    }

    raise(SIGUSR1);

    if(strcmp(argv[1], "pending") == 0){
        sigset_t check;
        
    }

    pid_t pid = fork();

    if(pid < 0){
        error();
    }
    else if(pid == 0){

    }
    else{
        
    }

    close(fd);

    return 0;
}