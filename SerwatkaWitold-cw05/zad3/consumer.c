#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    if(argc != 4){
        printf("Wrong number of arguments!\n Usage: ./consumer <path to pipe> <path to write file> <number of characters read from pipe>\n");
        exit(EXIT_FAILURE);
    }

    
}