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

struct matrix merge_matrixes(struct matrix dest, struct matrix src, int from, int to){
    for(int i = 0; i < dest.dim.height * dest.dim.width; i++){
        if((i + 1) % dest.dim.width >= from && (i + 1) % dest.dim.width < to){
            dest.values[i] = src.values[i];
        }
    }

    return dest;
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

//==================WRITING MATRIXES================//

void create_results_file(char* file_name, int matrix_width, int matrix_heigh){
    FILE* result_file = fopen(file_name, "w");
    if(result_file == NULL){
         printf("elo");
        error(EXIT_FAILURE);
    }

    rewind(result_file);
    char* safe_val = "0";

    for(int i = 0; i < matrix_heigh * matrix_width; i++){
        fwrite(safe_val, sizeof(char), 1, result_file);
        if((i + 1) % matrix_width == 0 && i + 1 != matrix_heigh * matrix_width){
            fwrite("\n", sizeof(char), 1, result_file);
        }
        else{
            fwrite(";", sizeof(char), 1, result_file);
        }
    }

    fclose(result_file);
}

int write_matrix(char* file_name, struct matrix src){
    FILE* result_file = fopen(file_name, "w");
    if(result_file == NULL){
        return -1;
    }

    rewind(result_file);
    char num[MAX_SIZE/4];

    for(int i = 0; i < src.dim.width * src.dim.height; i++){
        sprintf(num, "%d", src.values[i]);
        fwrite(num, sizeof(char), strlen(num), result_file);
        if((i + 1) % src.dim.width == 0){
            fwrite("\n", sizeof(char), 1, result_file);
        }
        else{
            fwrite(";", sizeof(char), 1, result_file);
        }
    }

    fclose(result_file);

    return 0;
}

//===============MATRIX MULTIPLICATION================//

void save_result_common(struct matrix matrix, char* file_name, int block_size, int block_number){
    //Step 1: read current results
    struct matrix results = read_matrix(file_name);

    //Step 2: merge matrixes
    results = merge_matrixes(results, matrix, block_number * block_size, (block_number + 1) * block_size);

    //Step 3: write new results
    if(write_matrix(file_name, results) == -1){
        error(EXIT_FAILURE);
    }
}

void save_result_separate(struct matrix matrix, char* file_name, int block_size, int block_number){
    struct matrix result;

    result.dim.width = block_size;
    result.dim.height = matrix.dim.height;
    result.values = calloc(result.dim.width * result.dim.height, sizeof(int));

    for(int i = 0; i < result.dim.width * result.dim.height; i++){
        result.values[i] = matrix.values[(block_size * block_number) + (i % matrix.dim.width) + matrix.dim.width * (i / block_size)];
    }

    char num[MAX_SIZE/4];
    sprintf(num, "%d", block_number);
    strcat(num, file_name);
    write_matrix(num, result);
}

void multiply_matrixes(struct matrix matrix1, struct matrix matrix2, int processes_number, int max_time, int common_flag){
    int counter = 0;
    pid_t child_pid[processes_number];
    int block_size = matrix2.dim.width/processes_number;

    if(common_flag == 1){
        create_results_file("results.txt", matrix2.dim.width, matrix1.dim.height);
    }

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
            result.dim.width = matrix2.dim.width;
            result.dim.height = matrix1.dim.height;
            result.values = calloc(result.dim.width * result.dim.height, sizeof(int));
            
            for(int i = counter * block_size; i < (counter + 1) * block_size; i++){ //kolumna macierzy b
                for(int j = 0; j < matrix1.dim.width; j++){                         //rząd macierzy a
                    for(int k = 0; k < matrix1.dim.width; k++){
                        result.values[result.dim.width * j + i] += matrix1.values[matrix1.dim.width * j + k] * matrix2.values[matrix2.dim.width * k + i];
                        m_counter++;
                    }
                }
            }

            if(common_flag == 1){
                save_result_common(result, "results.txt", block_size, counter);
            }
            else{
                printf("elo\n");
                save_result_separate(result, "result.txt", block_size, counter);
            }

            exit(m_counter);
        }
    }

    if(common_flag == 0){
        //złącz pliki poleceniem "paste"
        pid_t pasting = fork();

        if(pasting < 0){
            error(EXIT_FAILURE);
        }
        else if(pasting == 0){
            const char* argv[3 + processes_number];

            argv[0] = "paste";
            argv[1] = "-d=;";

            char num[MAX_SIZE/4];
            for(int i = 0; i < processes_number; i++){
                sprintf(num, "%d", i);
                strcat(num, "result.txt");
                argv[i + 2] = num;
            }

            // argv[processes_number + 2] = "&>";
            // argv[processes_number + 3] = "results.txt";
            argv[processes_number + 2] = NULL;

            execvp("paste", argv);


            //exit xd
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

    multiply_matrixes(A_matrix, B_matrix, child_processes, max_time, common_flag);

    return EXIT_SUCCESS;
}