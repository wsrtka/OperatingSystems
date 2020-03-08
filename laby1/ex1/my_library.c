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

struct File_pair create_file_pair(FILE first, FILE second){
    struct File_pair file_pair;

    file_pair.first = first;
    file_pair.second = second;

    return file_pair;
}

char* compare_files(struct File_pair pair){
    char* command = calloc(command_size, sizeof(char));
}