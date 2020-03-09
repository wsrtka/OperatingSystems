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

int create_block(struct Main_array arr, char* tmp_filename){
    FILE* tmp_file = fopen(tmp_filename, "r");
    if(tmp_file == NULL){
        fprintf(stderr, "Could not open temporary file");
        exit(1);
    }

    fseek(tmp_file, 0, SEEK_END);
    long file_length = ftell(tmp_file);
    fseek(tmp_file, 0, SEEK_SET);

    
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
