#include "settings.h"
#include "shared.h"

#include <unistd.h>

int semtab;
int* arr;
Counter* counter;

void close_shop(){



	if(semctl(semtab, 0, IPC_RMID, 0) == -1){
		error("Could not close semaphore array.");
	}
}

int create_sems(){
	key_t key = get_key(SEMAPHORE);
	
	int semid;
	if((semid = semget(key, SHOP_CAP, IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not initialize semaphore array.");
	}

	unsigned short init_val[SHOP_CAP] = {0};
	union semnum arg;
	arg.array = init_val;
	
	if(semctl(semid, 0, SETALL, arg) == -1){
		error("Could not initialize semaphore values.");
	}

	return semid;
}

int* create_array(){
	key_t key = get_key(ARRAY);

	int arrid;
	if((arrid = shmget(key, SHOP_CAP * sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not create shared array.");
	}

	int* arr = (int*) shmat(arrid, NULL, 0666);
	for(int i = 0; i < SHOP_CAP; i++){
		arr[i] = 0;
	}

	return arrid;
}

Counter* create_counter(){
	key_t key = get_key(COUNTER);

	int countid;
	if((countid = shget(key, sizeof(Counter), IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not create shared counter.");
	}

	Counter* counter = (Counter *) shmat(countid, NULL, 0666);
	counter -> to_prepare = 0;
	counter -> to_send = 0;

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
    }

	for(int i = 0; i < LOADER_NO; i++){
        pid_t pid = fork();

		if(pid == 0){
			execl("./loader", "./loader", NULL);
		}
		else if(pid < 0){
			error("Could not create loader.");
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
    }

    printf("Shop closing.\n");

	raise(SIGINT);

    return 0;
}