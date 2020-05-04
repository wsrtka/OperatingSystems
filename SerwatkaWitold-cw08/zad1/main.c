#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct thread_args {
    int* histogram;
    int from;
    int to;
    int img_width;
    int img_heigth;
    int** img;
} thread_args;

//==========HELPER FUNCTIONS============//
void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void read_file_header(FILE* file, int* width, int* height, int* maxval){
    rewind(file);

    char* line = NULL;
    char* value = NULL;
    size_t len = 0;

    for(int i = 0; i < 4; i++){
        if(getline(&line, &len, file) == -1){
            error("Could not read header file.");
        }

        switch (i)
        {
        case 0:
            if(strcmp(line, "P2\n") != 0){
                error("Invalid file header.");
            }
            break;

        case 1:
            value = strtok(line, " ");
            value = strtok(NULL, " ");
            printf("Reading file %s", value);
            break;

        case 2:
            value = strtok(line, " ");
            *width = atoi(value);
            value = strtok(NULL, " ");
            *height = atoi(value);
            break;

        case 3:
            *maxval = atoi(line);
            break;
        
        default:
            break;
        }
    }
}

void read_image(FILE* file, int width, int heigth, int** image){
    char* line = NULL;
    char* value = NULL;
    size_t len = 0;
    int i = 0, j = 0;

    while(getline(&line, &len, file) > 0){
        value = strtok(line, " ");

        do{
            image[i++][j] = atoi(value);

            if(i == width){
                i = 0;
                j++;
            }

            if(j == heigth){
                return;
            }
        } while((value = strtok(NULL, " ")) != NULL);
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

    //wczytanie metadanych
    FILE* input_file = fopen(IN_FILE, "r");
    if(input_file == NULL){
        error("Could not open input file.");
    }

    int temp_width, temp_height, temp_maxval;

    read_file_header(input_file, &temp_width, &temp_height, &temp_maxval);

    const int width = temp_width;
    const int height = temp_height;
    const int maxval = temp_maxval;

    if(width < 1 || height < 1 || maxval < 0){
        error("Invalid image metadata.");
    }

    int histogram[256];

    //ustawianie argumentów dla funkcji threads
    thread_args args;
    args.histogram = histogram;

    args.img = malloc(width * sizeof(int*));
    for(int i = 0; i < width; i++){
        args.img[i] = malloc(height * sizeof(int));
    }

    args.img_heigth = height;
    args.img_width = width;

    read_image(input_file, width, height, args.img);

    //wątkowanie
    

    return 0;
}