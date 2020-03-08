#ifndef my_library
#define my_library

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Block {
    char* operations;
    int size;
};

struct Main_array {
    struct Block** blocks;
    int size;
};

struct File_pair {
    FILE first;
    FILE second;
};

struct File_pair_array {
    struct File_pair* files;
    int size;
};

struct Main_array create_array(int size);
struct File_pair create_file_pair(FILE first, FILE second);
char* compare_files(struct File_pair pair);
bool save_to_temp_file(char* text);
int create_block(struct Main_array arr, FILE temp_file);
int get_operations_amount(struct Block block);
bool delete_block(struct Main_array arr, int id);
bool delete_operation(struct Block block, char* operation);

#endif