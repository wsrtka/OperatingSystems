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
                error("'generate' 1st argument should be string");
            }
            char* filename = calloc(strlen(argv[i]), sizeof(char));
            strcpy(filename, argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'generate' 2nd argument should be number");
            }
            int lines_count = is_number(argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'generate' 3rd argument should be number");
            }
            int bytes = is_number(argv[i++]);

            generate_file(filename, lines_count, bytes);
        }
        else if(strcmp(argv[i], "sort") == 0){

        }
        else if(strcmp(argv[i], "copy") == 0){

        }else{
            error("Could not understand argument");
        }
    }
}