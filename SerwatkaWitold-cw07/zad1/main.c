#include "settings.h"

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        error("Invalid number of arguments, usage: ./main <nr of workers>");
    }

    printf("Opening shop!\n");

    int num_of_workers = atoi(argv[1]);
    
	char* path;
	if((path = getenv("HOME")) == NULL){
		error("Could not get environment variable.");
	}

	key_t key;
	if((key = ftok(path, PROJECT)) == -1){
		error("Could not get object key.");
	}
	
	int semtab;
	if((semtab = semget(key, NSEMS, IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not initialize semaphore array.");
	}
    
    for(int i = 0; i < num_of_workers; i++){
        //po kolei odpala pracownikow
    }

    printf("Shop closing.\n");

	if(semctl(semtab, 0, IPC_RMID, 0) == -1){
		error("Could not close semaphore array.");
	}

    return 0;
}