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
    int* values;
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
    result.values = calloc(result.dim.width * result.dim.height, sizeof(int));

    char* num;
    char* buffer;
    size_t len = 0;
    int counter = 0;
    rewind(file);

    while(getline(&buffer, &len, file) != -1){
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

//===============MATRIX MULTIPLICATION================//

void save_result_common(struct matrix matrix, char* filename, int block_size, int block_number){
    FILE* file = fopen("result.txt", "r+"); //thread safe
    if(file == NULL){
        exit(EXIT_FAILURE);
    }

    char c;
    int curr_width = 0;
    rewind(file);

    do{
        c = (char)getc(file);
        if(c == ';'){
            curr_width++;
        }
    }
    while(c != '\n');

    fclose(file);
}

void multiply_matrixes(struct matrix matrix1, struct matrix matrix2, int processes_number, int max_time, int common_flag){
    int counter = 0;
    pid_t child_pid[processes_number];
    int block_size = matrix2.dim.width/processes_number;

    // FILE* file = fopen("result.txt", "r+");

    while(counter < processes_number){
        pid_t pid = fork();

        if(pid < 0){
            error(EXIT_FAILURE);
        }
        else if(pid != 0){
            child_pid[counter++] = pid;
        }
        else{
            int m_counter = 0;

            struct matrix result;
            result.dim.width = block_size;
            result.dim.height = matrix1.dim.height;
            result.values = calloc(result.dim.width * result.dim.height, sizeof(int));
            
            for(int i = counter * block_size; i < (counter + 1) * block_size; i++){
                for(int j = 0; j < matrix1.dim.width; j++){
                    for(int k = 0; k < matrix1.dim.width; k++){
                        result.values[result.dim.width * j + i - counter * block_size] += matrix1.values[matrix1.dim.width * j + k] * matrix2.values[matrix2.dim.width * k + i];
                        m_counter++;
                    }
                }
            }

            if(common_flag == 0){
                //zapis common
            }
            else{
                //zapis separate
            }

            exit(m_counter);
        }
    }
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
    struct matrix C_matrix;

    multiply_matrixes(A_matrix, B_matrix, child_processes, max_time, common_flag);

    return EXIT_SUCCESS;
}