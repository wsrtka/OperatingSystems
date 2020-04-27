#include "settings.h"
#include "shared.h"

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        error("Invalid number of arguments, usage: ./receiver <key>");
    }

    printf("Opening shop!\n");

	char* path = getenv("HOME");
	key_t key = ftok(path, SEMAPHORE);
	int semtab = semget(key, 0, 0666);

    int size;
    if((size = semctl(semtab, 0, GETVAL)) == -1){
        error("Could not get semaphore value.");
    }

    
}