#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "myfslib.h"

#define size 128

int is_number(char* str){
    char* check = calloc(size, sizeof(char));
    long val;

    val = strtol(str, check, 10);

    if(check == str || *check != '\0'){
        return -1;
    }
    else{
        return (int)val;
    }
}

int main(int argc, char** argv){

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i++], "generate") == 0){
            if(is_number(argv[i]) != -1){
                error("'generate' usage: generate <file name> <records> <bytes>");
            }
            char* filename = calloc(strlen(argv[i]), sizeof(char));
            strcpy(filename, argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'generate' usage: generate <file name> <records> <bytes>");
            }
            int lines_count = is_number(argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'generate' usage: generate <file name> <records> <bytes>");
            }
            int bytes = is_number(argv[i++]);

            generate_file(filename, lines_count, bytes);
        }
        else if(strcmp(argv[i++], "sort") == 0){
            if(is_number(argv[i]) != -1){
                error("'sort' usage: sort <file name> <records> <bytes> <lib | sys>");
            }
            char* filename = calloc(strlen(argv[i]), sizeof(char));
            strcpy(filename, argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'sort' usage: sort <file name> <records> <bytes> <lib | sys>");
            }
            int records = is_number(argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'sort' usage: sort <file name> <records> <bytes> <lib | sys>");
            }
            int bytes = is_number(argv[i++]);

            if(is_number(argv[i]) != -1){
                error("'sort' usage: sort <file name> <records> <bytes> <lib | sys>");
            }
            
            if(strcmp(argv[i], "sys") == 0){
                int fd = open(filename, O_RDWR);

                sys_qsort(fd, 0, records, bytes);
            }
            else if(strcmp(argv[i], "lib") == 0){
                FILE* file = fopen(filename, "a+");

                lib_qsort(file, 0, records, bytes);
            }
            else{
                error("'sort' usage: sort <file name> <records> <bytes> <lib | sys>");
            }

            i++;
        }
        else if(strcmp(argv[i++], "copy") == 0){
            if(is_number())
        }else{
            error("Could not understand argument");
        }
    }
}