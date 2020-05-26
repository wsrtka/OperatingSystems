#ifndef shared
#define shared

#include "settings.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

//data types
union semum{
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
};

typedef struct Counter{
    int to_prepare;
    int to_send;
    int max_id;
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

char* get_name(char* buf, int id){
    sprintf(buf, "/%d", id);
    return buf;
}

int get_semaphores(){
    int fd;
    if((fd = shm_open(SEMS_NAME, O_RDWR, 0666)) == -1){
        error("Could not get semaphore array.");
    }

    if(ftruncate(fd, SHOP_CAP * sizeof(sem_t*)) == -1){
        error("Could not truncate semaphore array.");
    }

    return fd;
}

sem_t** attach_semaphores(){
    int semfd = get_semaphores();
    sem_t** ptr;

    ptr = (sem_t*) mmap(NULL, SHOP_CAP * sizeof(sem_t*), PROT_READ | PROT_WRITE, MAP_SHARED, semfd, 0);
    if((int)ptr == -1){
        error("Could not get shared sem array.");
    }

    return ptr;
}

int get_array(){
    int fd;
    if((fd = shm_open(ARR_NAME, O_RDWR, 0666)) == -1){
        error("Could not get shared array fd.");
    }

    if(ftruncate(fd, SHOP_CAP * sizeof(Order)) == -1){
        error("Could not truncate shared array.");
    }

    return fd;
}

Order* attach_array(){
    int arrfd = get_array();
    Order* ptr;

    ptr = (Order*) mmap(NULL, SHOP_CAP * sizeof(Order), PROT_READ | PROT_WRITE, MAP_SHARED, arrfd, 0);
    if((int)ptr == -1){
        error("Could not get shared array.");
    }

    return ptr;
}

void detach(const void* addr, char* name, size_t len){
    if(munmap(addr, len) == -1){
        error("Could not detach.");
    }

    if(shm_unlink(name) == -1){
        error("Could not unlink.");
    }
}

int get_counter(){
    int fd;
    if((fd = shm_open(COUNTER_NAME, O_RDWR, 0666)) == -1){
        error("Could not get shared counter fd.");
    }

    if(ftruncate(fd, sizeof(Counter)) == -1){
        error("Could not truncate shared counter.");
    }

    return fd;
}

Counter* attach_counter(){
    int countfd = get_counter();
    Counter* ptr;

    ptr = (Counter*) mmap(NULL, sizeof(Counter), PROT_READ | PROT_WRITE, MAP_SHARED, countfd, 0);
    if((int)ptr == -1){
        error("Could not get shared counter.");
    }

    return ptr;
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