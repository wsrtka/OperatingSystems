#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//===================HELPER FUNCTIONS================//

void error(int exit_code){
    printf("Program usage: ./macierz <source file> <nr of child processes> <time>\n");
    exit(exit_code);
}

int main(int argc, char** argv){
    if(argc != 4){
        error(EXIT_FAILURE);
    }

    return 0;
}