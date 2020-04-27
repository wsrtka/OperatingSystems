#ifndef shared
#define shared

#include "settings.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

//data types
union semnum{
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
};

typedef struct Counter{
    int to_prepare;
    int to_send;
} Counter;

typedef struct Order{
    int num;
    int state;
} Order;

//utility functions
void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

key_t get_key(char identifier){
    char* path;
	if((path = getenv("HOME")) == NULL){
		error("Could not get environment variable.");
	}

	key_t key;
	if((key = ftok(path, identifier)) == -1){
		error("Could not get object key.");
	}

    return key;
}

int get_semaphores(){
    key_t key = get_key(SEMAPHORE);

    int semid;
	if((semid = semget(key, SHOP_CAP, 0666)) == -1){
		error("Could not initialize semaphore array.");
	}

    return semid;
}

int get_array(){
    key_t key = get_key(ARRAY);

    int arrid;
    if((arrid = shmget(key, 0, 0666)) == -1){
        error("Could not get array key.");
    }

    return arrid;
}

Order* attach_array(){
    int arrid = get_array();
    Order* ptr;

    ptr = (Order*) shmat(arrid, NULL, 0666);
    if((int)ptr == -1){
        error("Could not get shared array.");
    }

    return ptr;
}

void detach(const void* addr){
    if(shmdt(addr) == -1){
        error("Could not detach.");
    }
}

int get_counter(){
    key_t key = get_key(COUNTER);

    int arrid;
    if((arrid = shmget(key, 0, 0666)) == -1){
        error("Could not get array key.");
    }

    return arrid;
}

Counter* attach_counter(){
    int countid = get_counter();
    Counter* ptr;

    ptr = (Counter*) shmat(countid, NULL, 0666);
    if((int)ptr == -1){
        error("Could not get shared counter.");
    }

    return ptr;
}

//semaphore locking
void lock_semaphore(int semid, int num){
    struct sembuf sops[1];
    sops[0].sem_num = num;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    if(semop(semid, sops, 1) == -1){
        error("Unable to lock semaphore.");
    }
}

void unlock_semaphore(int semid, int num){
    struct sembuf sops[1];
    sops[0].sem_num = num;
    sops[0].sem_op = 1;
    sops[0].sem_flg = 0;

    if(semop(semid, sops, 1) == -1){
        error("Unable to unlock semaphore.");
    }
}

//getting timestamp
char* gettimestamp(char* buffer){
    char timestamp[82];

    struct timeval time;
    gettimeofday(&time, NULL);
    int mil = time.tv_usec / 1000;

    strftime(timestamp, 80, "%Y-%m-%d %H:%M:%S", localtime(&time.tv_sec));
    sprintf(buffer, "%s:%03d", timestamp, mil);

    return buffer;
}

#endif