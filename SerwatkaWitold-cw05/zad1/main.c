#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

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
            char** args = split_args(commands[i]);

            if(i != 0){
                close(pipes[(i+1)%2][1]);

                if(dup2(pipes[(i+1)%2][0], STDIN_FILENO) == -1){
                    printf("Could not redirect pipe input to stdin on run %d.\n", i);
                    exit(EXIT_FAILURE);
                }
            }

            if(i != commands_count - 1){
                close(pipes[i % 2][0]);

                if(dup2(pipes[i % 2][1], STDOUT_FILENO) == -1){
                    printf("Could not redirect stdout to pipe output on run %d.\n", i);
                    exit(EXIT_FAILURE);
                }
            }

            execvp(args[0], args);
        }
        else if(pid < 0){
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

    while((read = getline(&line, &len, source_file)) != -1){
        execute_line(line);
    }

    free(line);

    return 0;
}