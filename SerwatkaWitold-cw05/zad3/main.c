#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

int main(){
    printf("Setting up environment.\n");
    system("make setup");

    printf("Creating consumer.\n");

    pid_t pid = fork();
    if(pid == 0){
        srand(time(NULL));
        char number[2];
        sprintf(number, "%d", (rand()%15)+1);

        execlp("./consumer", "./consumer", "./pajposz", "./destFile.txt", number, NULL);
    }
    else if(pid < 0){
        printf("Unable to create consumer.\n");
        exit(EXIT_FAILURE);
    }

    printf("Creating 5 producers.\n");

    for(int i = 0; i < 5; i++){
        pid = fork();
        if(pid == 0){
            printf("Creating producer number %d.\n", i+1);

            char filename[23];
            sprintf(filename, "srcFile%d.txt", i+1);

            srand(time(NULL));
            char number[2];
            sprintf(number, "%d", (rand()%10)+1);

            execlp("./producer", "./producer", "./pajposz", filename, number, NULL);
        }
        else if(pid < 0){
            printf("Could not create producer number %d.\n", i+1);
            exit(EXIT_FAILURE);
        }
    }

    wait(NULL);
    printf("Operations successfull.\n");

    return 0;
}