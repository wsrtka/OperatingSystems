#include "my_library.h"

int size = 140;

struct Main_array create_array(int size){
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
    
}