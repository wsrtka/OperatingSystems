#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
// #include <sys/times.h>
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

double time_diff(clock_t time1, clock_t time2){
    return (double)(time2 - time1) / sysconf(CLOCKS_PER_SEC);
}

void write_result(clock_t start, clock_t end, struct tms* time_start, struct tms* time_end, FILE* result_file){
    printf("Real time: %fl\n", timeDifference(start,end));
    printf("User time: %fl\n", timeDifference(time_start->tms_utime, time_end->tms_utime));
    printf("System time: %fl\n", timeDifference(time_start->tms_stime, time_end->tms_stime));

    fprintf(result_file, "Real time: %fl\n", timeDifference(start, end));
    fprintf(result_file, "User time: %fl\n", timeDifference(time_start->tms_utime, time_end->tms_utime));
    fprintf(result_file, "System time: %fl\n", timeDifference(time_start->tms_stime, time_end->tms_stime));
}

int main(int argc, char** argv){
    if(argc < 2){
        error("Invalid number of arguments");
    }

    FILE* result_file = fopen("raport2.txt",'a');

    struct tms* system_times[argc];
    clock_t user_time[argc];
    for(int i = 0; i < argc; i++){
        system_times[i] = calloc(1, sizeof(struct tms*));
        user_time[i] = 0;
    }
    int counter = 0;

    struct Main_array array;

    for(int i = 1; i < argc; i++){
        user_time[counter] = times(system_times[counter]);
        counter++;

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
            while(!isCommand(argv[++i])){ //dopisać error handling na nieprawidłowe dane wejściowe
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
                printf("Created block number %d", create_block(&array, tmp_filename));
            }
        }
        else if(strcpm(argv[i], "remove_block") == 0)
        {
            if(!isNumber(argv[i+1])){
                error("Invalid argument: should be of type int");
            }

            printf("Proceeding to remove block no. %d", argv[i+1]);
            delete_block(&array, argv[i+1]);

            i += 2;
        }
        else if(strcpm(argv[i], "remove_operation") == 0)
        {
            if(!isNumber(argv[i+1]) || !isNumber(argv[i+2])){
                error("Invalid argument: should be of type int");
            }

            printf("Proceeding to remove operation no. %d from block no. %d", argv[i+1], argv[i+2]);
            delete_operation(&array, argv[i+1], argv[i+2]);

            i += 3;
        }
        else
        {
            error("Invalid argument");
        }
        user_time[counter] = times(system_times[counter]);

        write_result(user_time[counter - 1], user_time[counter], system_times[counter - 1], system_times[counter], result_file);
        counter++;
    }

    return 0;
}