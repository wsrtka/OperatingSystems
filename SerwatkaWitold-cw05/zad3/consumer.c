#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv){
    if(argc != 4){
        printf("Wrong number of arguments!\n Usage: ./consumer <path to pipe> <path to write file> <number of characters read from pipe>\n");
        exit(EXIT_FAILURE);
    }

    char* pipe = argv[1];
    char* dest_filename = argv[2];
    int n = atoi(argv[3]);

    int fd_pipe = open(pipe, O_RDONLY);
    if(fd_pipe < 0){
        print("Ubnable to open pipe.\n");
        exit(EXIT_FAILURE);
    }

    int fd_dest = open(dest_filename, O_WRONLY);
    if(fd_dest < 0){
        print("Unable to open destination file.\n");
        exit(EXIT_FAILURE);
    }

    lseek(fd_pipe, 0, SEEK_SET);
    lseek(fd_dest, 0, SEEK_SET);

    char* buffer = calloc(n, sizeof(char));
    
    while(read(fd_pipe, &buffer, n) != 0){
        //czytać samą treść pliku początkowego czy razem z pid producentów?
        if(write(fd_dest, &buffer, n) == 0){
            print("Unable to write to destination file.\n");
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
    close(fd_pipe);
    close(fd_dest);

    return 0;
}