#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 128

char** split_args(char* command){
    int counter = 0;
    char** args = NULL;
    char delims[2] = {' ', '\n'};
    char* word = NULL;

    word = strtok(command, delims);

    while(word != NULL){
        args = realloc(args, ++counter*sizeof(char*));
        args[counter - 1] = word;
        word = strtok(NULL, delims);
    }

    args = realloc(args, (counter + 1)*sizeof(char*));
    args[counter] = NULL;

    return args;
}

void execute_line(char* line){
    int fork_count = 0;
    int commands_count = 0;
    int pipes[2][2];
    char* commands[SIZE];
    char* command = NULL;

    command = strtok(line, "|");
    while(command != NULL){
        commands[commands_count++] = command;
        command = strtok(NULL, "|");
    }

    for(int i = 0; i < commands_count; i++){
        if(i != 0){
            close(pipes[i % 2][0]);
            close(pipes[i % 2][1]);
        }
        
        if(pipe(pipes[i % 2]) == -1){
            printf("Unable to create pipe on run %d.\n", i);
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();

        if(pid == 0){
            //dziecko robi
        }
        else if(pid > 0){
            //rodzic czeka
        }
        else{
            printf("Fork error on run %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

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
    int counter = 0;
    int forks = 0;
    char* args[SIZE] = { NULL };
    char* arg = NULL;

    while((read = getline(&line, &len, source_file)) != -1){

        char delims[2] = {' ', '\n'};
        arg = strtok(line, delims);

        while(arg != NULL){
            if(strcmp(arg, "|") == 0){
                int fd[2];
                pipe(fd);

                int pid = fork();
                if(pid == 0){
                    dup2(fd[0], STDIN_FILENO);
                    dup2(STDOUT_FILENO, fd[1]);
                }

                counter = 0;
                char* args[SIZE] = { NULL };
            }
            else{
                args[counter++] = arg;

                arg = strtok(NULL, delims);
            }
        }
    }

    free(line);

    return 0;
}