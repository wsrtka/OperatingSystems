#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//==========HELPER FUNCTIONS============//
void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

//==========SIGN MODE============//

//==========BLOCK MODE============//

//==========INTERLEAVED MODE============//

//==========MAIN============//
int main(int argc, char* argv[]){
    //kontrola argumentów
    if(argc != 5){
        error("Invalid number of arguments.");
    }

    //wczytanie argumentow
    const int THREADS_NO = atoi(argv[1]);
    const char* MODE = argv[2];
    const char* IN_FILE = argv[3];
    const char* OUT_FILE = argv[4];

    //wczytanie danych
    FILE* input_file = fopen(IN_FILE, "r");
    if(input_file == NULL){
        error("Could not open input file.");
    }

    

    return 0;
}