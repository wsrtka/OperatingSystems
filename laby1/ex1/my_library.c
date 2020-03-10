#include "my_library.h"

int command_size = 256;

struct Main_array create_array(int size){
    if(size <= 0){
        fprintf(stderr, "Argument size must be >= 0");
        exit(1);
    }

    struct Main_array array;

    array.blocks = (struct Block*) calloc(size, sizeof(struct Block));
    array.size = 0;

    return array;
}

struct File_pair create_file_pair(char* first, char* second){
    struct File_pair file_pair;

    file_pair.first = first;
    file_pair.second = second;

    return file_pair;
}

char* compare_files(struct File_pair pair){
    char* command = calloc(command_size, sizeof(char));

    char* tmp_filename = tmpnam(NULL);

    strcpy(command, "diff ");
    strcpy(command, pair.first);
    strcpy(command, " ");
    strcpy(command, pair.second);
    strcpy(command, " > ");
    strcpy(command, tmp_filename);

    system(command);

    return tmp_filename;
}

int create_block(struct Main_array* arr, char* tmp_filename){
    FILE* tmp_file = fopen(tmp_filename, "r");
    if(tmp_file == NULL){
        fprintf(stderr, "Could not open temporary file");
        exit(1);
    }

    fseek(tmp_file, 0, SEEK_END);
    long file_length = ftell(tmp_file);
    fseek(tmp_file, 0, SEEK_SET);

    int file_lines = lines_count(tmp_file);
    
    struct Block result;
    struct Block* result_ptr = &result;
    result.size = -1;
    result.operations = (char*) calloc(file_lines/2, sizeof(char*));

    char* line = NULL;
    size_t len = 0;
    int operation_length;

    while(getline(&line, &len, tmp_file) != -1){
        if(isdigit(atoi(line[0]))){
            operation_length = 0;
            result.size++;
            result.operations[result.size] = calloc(len, sizeof(char));
            operation_length += len;
        }
        else{
            operation_length += len;
            result.operations[result.size] = realloc(result.operations[result.size], operation_length*sizeof(char));
        }

        strcat(result.operations[result.size], line);
    }

    arr->blocks[arr->size] = result_ptr;
    arr->size++;

    return arr->size-1;
}

int get_operations_amount(struct Block block){
    return block.size;
}

bool delete_block(struct Main_array* arr, int id){
    struct Block* block = arr->blocks[id];

    for(int i = 0; i < block->size; i++){
        free(block->operations[i]);
    }

    free(arr->blocks[id]);

    if(id < arr->size - 1){
        struct Block* prev = arr->blocks[id];
        struct Block* next = arr->blocks[id+1];

        for(int i = id; i < arr->size - 1; i++){
            arr->blocks[i] = arr->blocks[i+1];
        }

        arr->blocks[arr->size - 1] = NULL;
    }

    arr->size--;

    return true;
}

bool delete_operation(struct Main_array* array, int block_id, int operation_id){
    if(block_id > array->size){
        error("Invalid block id");
    }

    struct Block* block = array->blocks[block_id];

    if(operation_id > block->size){
        fprintf(stderr, "invalid operation index");
        exit(1);
    }

    free(block->operations[operation_id]);

    if(operation_id < block->size){
        for(int i = operation_id; i < block->size; i++){
            block->operations[i] = realloc(block->operations[i], sizeof(block->operations[i+1]));
            strcpy(block->operations[i], block->operations[i+1]);
        }
    }

    block->size--;

    return true;
}

int lines_count(FILE* filename){
    if(filename == NULL){
        return 0;
    }

    int lines = 0;
    char ch;
    while(!feof(filename)){
        ch = fgetc(filename);
        if(ch == '\n'){
            lines++;
        }
    }

    return lines;
}

void error(char* message){
    fprintf(stderr, message);
    exit(0);
}
