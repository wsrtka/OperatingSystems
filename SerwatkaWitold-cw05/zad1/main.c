#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong arguments count!\n Usage: ./main <file path>\n");
        exit(EXIT_FAILURE);
    }

    char* file_path = argv[1];
    FILE* source_file = fopen(file_path, "r");
    if(source_file == NULL){
        printf("Unable to open file.\n");
        exit(EXIT_FAILURE);
    }

    rewind(source_file);

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    while((read = getline(&line, &len, source_file)) != -1){
        
            //nie chrzanić się z tym, lecimy ze standardową biblioteką żeby obrobić pliki
            //czytamy jedną linijkę
            //dzielimy ją po spacjach
            //do nowej tablicy wrzucamy argumenty wywołania programu
            //wywołujemy program pamiętając o pipe

    }

    free(line);

    return 0;
}