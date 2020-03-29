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
    printf("Program usage: ./main <fork|exec> <option>\n");
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
    if(argc != 3){
        printf("Nieprawidłowa liczba argumentów\n");
        error();
    }

    // fd = open("raport2.txt", O_WRONLY | O_APPEND);
    // if(fd == -1){
    //     printf("Nie udało się otworzyć pliku 'raport2.txt'\n");
    // }

    int exec_flag;
    if(strcmp(argv[1], "fork") == 0){
        exec_flag = 0;
    }
    else if(strcmp(argv[1], "exec") == 0){
        exec_flag = 1;
    }
    else{
        error();
    }

    signal(SIGUSR1, sig_handler);

    if(strcmp(argv[2], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }
    else if(strcmp(argv[2], "handler") == 0){
        signal(SIGUSR1, handler);
    }
    else if(strcmp(argv[2], "mask") == 0 || strcmp(argv[2], "pending") == 0){
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

    sigset_t check;

    if(strcmp(argv[2], "ignore") == 0){
        printf("Signal is being ignored in parent\n");
    }
    else if(strcmp(argv[2], "mask") == 0){
        printf("Signal is being masked in parent\n");
    }
    else if(strcmp(argv[2], "pending") == 0){
        sigpending(&check);
        if(sigismember(&check, SIGUSR1) == 1){
            printf("SIGUSR1 is pending in parent\n");
        }
        else{
            printf("SIGUSR1 is not pending in parent");
        }
    }

    pid_t pid = fork();

    if(pid < 0){
        printf("Fork error\n");
        error();
    }
    else if(pid == 0){
        if(exec_flag == 0){
            if(strcmp(argv[2], "pending") == 0){
                sigpending(&check);
                if(sigismember(&check, SIGUSR1) == 1){
                    printf("Child reports: SIGUSR1 is pending in parent\n");
                }
                else{
                    printf("Child reports: SIGUSR1 is not pending in parent");
                }
            }

            raise(SIGUSR1);

            if(strcmp(argv[2], "ignore") == 0){
                printf("Signal is being ignored in child\n");
            }
            else if(strcmp(argv[2], "mask") == 0){
                printf("Signal is being masked in child\n");
            }

            exit(0);
        }
        else{
            
        }
    }

    // close(fd);

    return 0;
}