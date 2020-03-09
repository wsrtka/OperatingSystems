#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <my_library.h>

bool isNumber(char* var){
    for(int i = 0; i < strlen(var); i++){
        if((int)var[i] < 48 && (int)var[i] > 57){
            return false;
        }
    }

    return true;
}

bool isCommand(char* str){
    return strcmp(str, "create_table") == 0
        || strcmp(str, "compare_pairs") == 0
        || strcmp(str, "remove_block") == 0
        || strcmp(str, "remove_operation") == 0;
}

int main(int argc, char** argv){
    if(argc < 2){
        error("Invalid number of arguments");
    }

    struct Main_array array;

    for(int i = 1; i < argc; i++){
        if(strcpm(argv[i], "create_table") == 0){
            if(!isNumber(argv[i+1])){
                error("Invalid argument: should be of type int");
            }

            printf("Creating table of size %d\n", argv[i+1]);

            array = create_array(argv[i+1]);
            i += 2;
        }
        else if(strcpm(argv[i], "compare_pairs") == 0)
        {
            while(!isCommand(argv[++i])){
                char* input = calloc(strlen(argv[i]), sizeof(char));
                char* file1, file2;
                int name_length = 0;

                for(int j = 0; j < strlen(argv[i]); j++){
                    if(argv[i][j] == ':'){
                        file1 = calloc(j-1, sizeof(char));
                        file2 = calloc(strlen(argv[i]) - j, sizeof(char));

                        strcpy(file1, input);
                        strcpy(input, "");
                        name_length = 0;
                    }
                    else{
                        input[name_length++] = argv[j];
                    }
                }

                strcpy(file2, input);
                struct File_pair pair = create_file_pair(file1, file2);
                
                free(input);
                free(file1);
                free(file2);

                char* tmp_filename = compare_files(pair);
                printf("Created block number %d", create_block(array, tmp_filename));
            }
        }
        else if(strcpm(argv[i], "remove_block") == 0)
        {
            if(!isNumber(argv[i+1])){
                error("Invalid argument: should be of type int");
            }

            printf("Proceeding to remove block no. %d", argv[i+1]);
            delete_block(array, argv[i+1]);

            i += 2;
        }
        else if(strcpm(argv[i], "remove_operation") == 0)
        {
            if(!isNumber(argv[i+1]) || !isNumber(argv[i+2])){
                error("Invalid argument: should be of type int");
            }

            printf("Proceeding to remove operation no. %d from block no. %d", argv[i+1], argv[i+2]);
            delete_operation(array, argv[i+1], argv[i+2]);

            i += 3;
        }
        else
        {
            error("Invalid argument");
        }
        
        return 0;
    }
}