#include "settings.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        error("Invalid number of arguments, usage: ./main <nr of workers>");
    }

    printf("Opening shop!\n");

    int num_of_workers = atoi(argv[1]);

    for(int i = 0; i < num_of_workers; i++){
        //po kolei odpala pracownikow
    }

    printf("Shop closing.\n");

    return 0;
}