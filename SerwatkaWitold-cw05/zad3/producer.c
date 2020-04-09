#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

int main(int argc, char** argv){
    if(argc != 4){
        printf("Wrong number of arguments!\n Usage: ./producer <path to pipe> <path to file> <number of characters>\n");
        exit(EXIT_FAILURE);
    }

    char* pipe = argv[1];
    char* src_filename = argv[2];
    int n = atoi(argv[3]);

    int fd_pipe = open(pipe, O_RDONLY);
    if(fd_pipe < 0){
        print("Ubnable to open pipe.\n");
        exit(EXIT_FAILURE);
    }

    int fd_src = open(src_filename, O_WRONLY);
    if(fd_src < 0){
        print("Unable to open destination file.\n");
        exit(EXIT_FAILURE);
    }

    lseek(fd_pipe, 0, SEEK_SET);
    lseek(fd_src, 0, SEEK_SET);

    char* buffer = calloc(n, sizeof(char));
    char* prefix = calloc(8, sizeof(char)); //8 = # + 6 cyfr pid + #
    sprintf(prefix, "#%d#", getpid());
    char* to_write = calloc(8+n, sizeof(char));
    
    while(read(fd_src, &buffer, n) != 0){
        strcpy(to_write, prefix);
        strcat(to_write, buffer);

        while(flock(fd_pipe, LOCK_EX) != 0){}

        if(write(fd_pipe, &to_write, strlen(to_write)) == 0){
            print("Unable to write to pipe.\n");
            exit(EXIT_FAILURE);
        }

        flock(fd_pipe, LOCK_UN);
    }

    free(buffer);
    free(prefix);
    free(to_write);
    close(fd_pipe);
    close(fd_src);

    return 0;
}