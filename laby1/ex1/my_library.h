#ifndef my_library
#define my_library

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Block {
    char** operations;
    int size;
};

struct Main_array {
    struct Block** blocks;
    int size;
};

struct File_pair {
    char* first;
    char* second;
};

struct Main_array create_array(int size);
struct File_pair create_file_pair(char* first, char* second);
char* compare_files(struct File_pair pair);
int create_block(struct Main_array arr, char* tmp_filename);
int get_operations_amount(struct Block block);
bool delete_block(struct Main_array arr, int id);
bool delete_operation(struct Block block, char* operation);

#endif