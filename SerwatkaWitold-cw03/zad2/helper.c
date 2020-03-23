#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 256

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
    printf("Program usage: ./helper <number> <min> <max>\n");
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

//=================MATRIX GENERATION=================//

int generate_number(){
    return (random() % 201) - 100;
}

char* generate_matrix(int width, int height, int count){
    char file_name[] = "./tests/matrix";
    char num[MAX_SIZE];
    sprintf(num, "%d", count);
    strcat(file_name, num);
    strcat(file_name, ".txt");

    FILE* result_file = fopen(file_name, "w");
    if(result_file == NULL){
        error(EXIT_FAILURE);
    }

    rewind(result_file);

    for(int i = 0; i < width * height; i++){
        sprintf(num, "%d", generate_number());
        fwrite(num, sizeof(char), strlen(num), result_file);
        if((i + 1) % width == 0){
            fwrite("\n", sizeof(char), 1, result_file);
        }
        else{
            fwrite(";", sizeof(char), 1, result_file);
        }
    }

    fclose(result_file);

    return file_name;
}

//=================MATRIX VERIFICATION===============//

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
    printf("tutaj?");
    FILE* file = fopen(file_name, "r");
    if(file == NULL){
        printf("tam?");
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

void assert_multiplication_result(struct matrix A_matrix, struct matrix B_matrix, struct matrix C_matrix){
    int control_sum = 0;

    for(int i = 0; i < B_matrix.dim.width; i++){                //kolumna macierzy b
        for(int j = 0; j < A_matrix.dim.width; j++){            //rząd macierzy a
            for(int k = 0; k < A_matrix.dim.width; k++){
                control_sum += A_matrix.values[A_matrix.dim.width * j + k] * B_matrix.values[B_matrix.dim.width * k + i];
            }
            if(control_sum != C_matrix.values[i * C_matrix.dim.width + j]){
                printf("Multiplication assertion failed!\n");
                exit(EXIT_FAILURE);
            }
            printf("Multiplication assertion on C[%d][%d] successfull.\n",j ,i);
        }
    }

    printf("All checks completed, matrix multiplication valid.\n\n");
}

//=========================MAIN======================//

int main(int argc, char* argv[]){
    if(argc != 4 || is_number(argv[1]) == 0 || is_number(argv[2]) == 0 || is_number(argv[3]) == 0 || atoi(argv[1]) < 1){
        error(EXIT_FAILURE);
    }

    int matrix_pairs = atoi(argv[1]);
    int min_size = atoi(argv[2]);
    int max_size = atoi(argv[3]);

    if(min_size < 1){
        min_size = 1;
    }

    if(max_size < min_size){
        int a = max_size;
        max_size = min_size;
        min_size = a;
    }

    int width_a, height_a, width_b;
    char* A_matrix_file;
    char* B_matrix_file;

    for(int i = 0; i < matrix_pairs * 2; i += 2){
        width_a = random() % (max_size - min_size) + min_size;
        width_b = random() % (max_size - min_size) + min_size;
        height_a = random() % (max_size - min_size) + min_size;

        A_matrix_file = generate_matrix(width_a, height_a, i);
        B_matrix_file = generate_matrix(width_b, width_a, i + 1);

        printf("tu?");

        assert_multiplication_result(read_matrix(A_matrix_file), read_matrix(B_matrix_file), read_matrix("./results.txt"));
    }
}