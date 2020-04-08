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
    int fd = open(file_path, O_RDONLY);
    if(fd == -1){
        printf("Unable to open file.\n");
        exit(EXIT_FAILURE);
    }

    lseek(fd, 0, SEEK_SET);

    int counter = 0;
    char c = 'l';
    char* buffer = calloc(0, sizeof(char));
    while(read(fd, &c, 1) != 0){
        if(c == '\n'){
            lseek(fd, -counter, SEEK_CUR);
            buffer = realloc(buffer, counter*sizeof(char));
            read(fd, &buffer, counter);

            counter = 0;
        }
        else{
            counter++;
        }
    }

    return 0;
}