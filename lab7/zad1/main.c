#include "settings.h"
#include "shared.h"

#include <signal.h>

sem_t** semtab;
Order* arr;
Counter* counter;
pid_t child_pids[RECEIVER_NO + LOADER_NO + SENDER_NO];

void close_shop(){
	for(int i = 0; i < RECEIVER_NO + LOADER_NO + SENDER_NO; i++){
		if(kill(child_pids[i], SIGINT) == -1){
			printf("Failed to tell worker %d to go home.\n", i);
		}
	}

	char name[8];
	for(int i = 0; i < SHOP_CAP; i++){
		sem_close(semtab[i]);
		sem_unlink(get_name(name, i));
	}
	if(munmap(semtab, SHOP_CAP * sizeof(sem_t*))== -1){
		printf("Could not detach shared sem array.\n");
		printf("%s\n", strerror(errno));
	}
	if(shm_unlink(SEMS_NAME) == -1){
		printf("Could not close shared sem array.\n");
		printf("%s\n", strerror(errno));
	}

	if(munmap(arr, SHOP_CAP * sizeof(Order))== -1){
		printf("Could not detach shared array.\n");
		printf("%s\n", strerror(errno));
	}
	if(shm_unlink(ARR_NAME) == -1){
		printf("Could not close shared array.\n");
		printf("%s\n", strerror(errno));
	}

	if(munmap(counter, sizeof(Counter)) == -1){
		printf("Could not detach shared array.\n");
		printf("%s\n", strerror(errno));
	}
	if(shm_unlink(COUNTER_NAME) == -1){
		printf("Could not close shared array.\n");
		printf("%s\n", strerror(errno));
	}

	exit(0);
}

sem_t** create_sems(){
	int fd;
	if((fd = shm_open(SEMS_NAME, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1){
		error("Could not create semaphore array.");
	}

	if(ftruncate(fd, SHOP_CAP * sizeof(sem_t*)) == -1){
		error("Could not set semaphore array size.");
	}

	sem_t** sems = (sem_t**) mmap(NULL, SHOP_CAP * sizeof(sem_t*), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

	char name[8];
	for(int i = 0; i < SHOP_CAP; i++){
		get_name(name, i);
		sems[i] = sem_open(name, O_RDWR, MAP_SHARED, 1);
	}

	return sems;
}

Order* create_array(){
	int fd;
	if((fd = shm_open(ARR_NAME, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1){
		error("Could not create shared array.");
	}

	if(ftruncate(fd, SHOP_CAP * sizeof(Order)) == -1){
		error("Could not set array size.");
	}

	Order* arr = (Order*) mmap(NULL, SHOP_CAP * sizeof(Order), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	for(int i = 0; i < SHOP_CAP; i++){
		arr[i].num = 0;
		arr[i].state = -1;
	}

	return arr;
}

Counter* create_counter(){
	int fd;
	if((fd = shm_open(COUNTER_NAME, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1){
		error("Could not create shared counter.");
	}

	if(ftruncate(fd, sizeof(Counter)) == -1){
		error("Could not set counter size.");
	}

	Counter* counter = (Counter *) mmap(NULL, SHOP_CAP * sizeof(Order), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	counter -> to_prepare = 0;
	counter -> to_send = 0;
	counter -> max_id = 0;

	return counter;
}

int main(){
    printf("Opening shop!\n");
	
	atexit(close_shop);
	signal(SIGINT, close_shop);

	semtab = create_sems();
	arr = create_array();
	counter = create_counter();
    
    for(int i = 0; i < RECEIVER_NO; i++){
        pid_t pid = fork();

		if(pid == 0){
			execl("./receiver", "./receiver", NULL);
		}
		else if(pid < 0){
			error("Could not create receiver.");
		}
		else{
			child_pids[i] = pid;
		}
    }

	for(int i = 0; i < LOADER_NO; i++){
        pid_t pid = fork();

		if(pid == 0){
			execl("./loader", "./loader", NULL);
		}
		else if(pid < 0){
			error("Could not create loader.");
		}
		else{
			child_pids[RECEIVER_NO + i] = pid;
		}
    }

	for(int i = 0; i < SENDER_NO; i++){
        pid_t pid = fork();

		if(pid == 0){
			execl("./sender", "./sender", NULL);
		}
		else if(pid < 0){
			error("Could not create sender.");
		}
		else{
			child_pids[RECEIVER_NO + LOADER_NO + i] = pid;
		}
    }

	sleep(60);

    printf("Shop closing.\n");

	raise(SIGINT);

    return 0;
}