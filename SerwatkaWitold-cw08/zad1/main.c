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

void read_file_header(FILE* file, int* width, int* height, int* maxval){
    char* line = NULL;
    char* value = NULL;

    for(int i = 0; i < 4; i++){
        if(getline(&line, NULL, file) == -1){
            error("Could not read header file.");
        }

        switch (i)
        {
        case 0:
            if(strcmp(line, "P2") != 0){
                error("Invalid file header.");
            }
            break;

        case 1:
            value = strtok(line, " ");
            value = strtok(NULL, " ");
            printf("Reading file %s.\n", value);
            break;

        case 2:
            value = strtok(line, " ");

            value = strtok(NULL, " ");
            break;

        case 3:
            
            break;
        
        default:
            break;
        }
    }
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

    int temp_width, temp_height, temp_maxval;
    read_file_header(input_file, &temp_width, &temp_height, &temp_maxval);

    return 0;
}