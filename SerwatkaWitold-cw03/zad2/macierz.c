#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 1024

struct matrix_dim{
    int width;
    int height;
};

struct matrix{
    short* values;
    struct matrix_dim dim;
};

//===================HELPER FUNCTIONS================//

void error(int exit_code){
    printf("Program usage: ./macierz <source file> <nr of child processes> <time> <common|separate>\n");
    exit(exit_code);
}

int is_number(char* str){
    for(int i = 0; i < strlen(str); i++){
        if(isdigit(str[i]) == 0){
            return 0;
        }
    }
    return 1;
}

//==================READING MATRIXES================//

struct matrix_dim get_matrix_size(FILE* file){
    int width = 0, height = 0;
    char c;
    do{
        c = (char)fgetc(file);
        if(c == ';' && height == 0){
            if(width == 0){
                width++;
            }
            width++;
        }
        if(c == '\n'){
            if(height == 0){
                height++;
            }
            height++;
        }
    }while(c != EOF);

    struct matrix_dim result;
    result.height = height;
    result.width = width;

    return result;
}

struct matrix read_matrix(char* file_name){
    FILE* file = fopen(file_name, "r");
    if(file == NULL){
        error(EXIT_FAILURE);
    }

    struct matrix_dim dim = get_matrix_size(file);
    struct matrix result;

    result.dim = dim;
    result.values = calloc(result.dim.width * result.dim.height, sizeof(short));

    char* num;
    char* buffer;
    size_t len = 0;
    ssize_t read;
    int counter = 0;
    rewind(file);

    while(read = getline(&buffer, &len, file) != -1){
        num = strtok(buffer, ";");
        do{
            result.values[counter++] = atoi(num);
            num = strtok(NULL, ";");
        }
        while(num != NULL);
    }

    free(buffer);

    fclose(file);

    return result;
}

//========================MAIN=======================//

int main(int argc, char** argv){
    if(argc != 5 || is_number(argv[1]) == 1 || is_number(argv[2]) == 0 || is_number(argv[3]) == 0 || is_number(argv[4]) == 1){
        error(EXIT_FAILURE);
    }

    char* source_file_name = argv[1];

    int child_processes = atoi(argv[2]);
    int max_time = atoi(argv[3]);
    int common_flag;
    strcmp(argv[4], "common") == 0 ? common_flag = 1 : strcmp(argv[4], "separate") == 0 ? common_flag = 0 : error(EXIT_FAILURE);

    FILE* source_file = fopen(source_file_name, "r");
    if(source_file == NULL){
        error(EXIT_FAILURE);
    }

    char A_matrix_file_name[MAX_SIZE], B_matrix_file_name[MAX_SIZE], C_matrix_file_name[MAX_SIZE];

    rewind(source_file);
    fscanf(source_file, "%s %s %s", A_matrix_file_name, B_matrix_file_name, C_matrix_file_name);
    fclose(source_file);

    struct matrix A_matrix = read_matrix(A_matrix_file_name);
    struct matrix B_matrix = read_matrix(B_matrix_file_name);
    // struct matrix C_matrix = read_matrix(C_matrix_file_name);

    return EXIT_SUCCESS;
}