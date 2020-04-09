#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong number of arguments!\n Usage: ./main <file name>\n");
        exit(EXIT_FAILURE);
    }

    char* command = calloc(5+strlen(argv[1]), sizeof(char));
    strcat(command, "sort ");
    strcat(command, argv[1]);

    FILE* sort_out = popen(command, "r");
    
    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    while((read = getline(&line, &len, sort_out)) != -1){
        printf("%s", line);
    }

    pclose(sort_out);

    return 0;
}