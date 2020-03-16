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
    struct tms* time_start = calloc(1, sizeof(struct tms*));
    struct tms* time_stop = calloc(1, sizeof(struct tms*));

    FILE* result_file = fopen("wyniki.txt", "a");

    for(int i = 1; i < argc; i++){
        times(time_start);

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

            times(time_stop);
            write_result(result_file, time_start, time_stop, "Time measurement for generate function:");
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

                times(time_stop);
                write_result(result_file, time_start, time_stop, "Time measurement for sys quicksort:");

                close(fd);
            }
            else if(strcmp(argv[i], "lib") == 0){
                FILE* file = fopen(filename, "a+");

                lib_qsort(file, 0, records, bytes);

                times(time_stop);
                write_result(result_file, time_start, time_stop, "Time mesurement for lib quicksort:");

                fclose(file);
            }
            else{
                error("'sort' usage: sort <file name> <records> <bytes> <lib | sys>");
            }

            i++;
        }
        else if(strcmp(argv[i++], "copy") == 0){
            if(is_number(argv[i]) != -1){
                error("'copy' usage: copy <file1> <file2> <records> <bytes> <lib | sys>");
            }
            char* file1 = calloc(strlen(argv[i]), sizeof(char));
            strcpy(file1, argv[i++]);

            if(is_number(argv[i]) != -1){
                error("'copy' usage: copy <file1> <file2> <records> <bytes> <lib | sys>");
            }
            char* file2 = calloc(strlen(argv[i]), sizeof(char));
            strcpy(file2, argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'copy' usage: copy <file1> <file2> <records> <bytes> <lib | sys>");
            }
            int records = is_number(argv[i++]);

            if(is_number(argv[i]) == -1){
                error("'copy' usage: copy <file1> <file2> <records> <bytes> <lib | sys>");
            }
            int bytes = is_number(argv[i++]);

            if(is_number(argv[i]) != -1){
                error("'copy' usage: copy <file1> <file2> <records> <bytes> <lib | sys>");
            }

            if(strcmp(argv[i], "sys") == 0){
                int fd1 = open(file1, O_RDONLY);
                int fd2 = open(file2, O_RDWR | O_APPEND);

                if(fd1 == -1 || fd2 == -1){
                    error("Could not open file");
                }

                sys_copy(fd1, fd2, records, bytes);

                times(time_stop);
                write_result(result_file, time_start, time_stop, "Time measurement for sys copy:");

                close(fd1);
                close(fd2);
            }
            else if(strcmp(argv[i], "lib") == 0){
                FILE* file1 = fopen(file1, "r");
                FILE* file2 = fopen(file2, "a+");

                if(file1 == NULL || file2 == NULL){
                    error("Could not open file");
                }

                lib_copy(file1, file2, records, bytes);

                times(time_stop);
                write_result(result_file, time_start, time_stop, "Time measurement for lib copy:");

                fclose(file1);
                fclose(file1);
            }
            else{
                error("'copy' usage: copy <file1> <file2> <records> <bytes> <lib | sys>");
            }

            i++;
        }else{
            error("Could not understand argument");
        }
    }
}