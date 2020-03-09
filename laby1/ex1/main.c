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

int main(int argc, char** argv){
    if(argc < 2){
        error("Invalid number of arguments");
    }

    for(int i = 1; i < argc; i++){
        if(strcpm(argv[i], "create_table") == 0){
            if()
        }
        else if(strcpm(argv[i], "compare_pairs") == 0)
        {
            
        }
        else if(strcpm(argv[i], "remove_block") == 0)
        {
            
        }
        else if(strcpm(argv[i], "remove_operation") == 0)
        {
            
        }
        else
        {
            error("Invalid argument");
        }
        
        return 0;
    }
}