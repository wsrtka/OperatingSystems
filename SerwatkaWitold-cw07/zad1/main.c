#include "settings.h"
#include "shared.h"

#include <signal.h>

int semtab;
int arrid;
int* arr;
int countid;
Counter* counter;
pid_t child_pids[RECEIVER_NO + LOADER_NO + SENDER_NO];

void close_shop(){
	// for(int i = 0; i < RECEIVER_NO + LOADER_NO + SENDER_NO; i++){
	// 	if(kill(child_pids[i], SIGINT) == -1){
	// 		printf("Failed to tell worker %d to go home.\n", i);
	// 	}
	// }

	//invalid argument in all??

	if(semctl(semtab, 0, IPC_RMID) == -1){
		printf("Could not close semaphore array.\n");
		printf("%s\n", strerror(errno));
	}

	if(shmdt(arr) == -1){
		printf("Could not detach shared array.\n");
		printf("%s\n", strerror(errno));
	}
	if(shmctl(arrid, IPC_RMID, NULL) == -1){
		printf("Could not close shared array.\n");
		printf("%s\n", strerror(errno));
	}

	if(shmdt(counter) == -1){
		printf("Could not detach shared array.\n");
		printf("%s\n", strerror(errno));
	}
	if(shmctl(countid, IPC_RMID, NULL) == -1){
		printf("Could not close shared array.\n");
		printf("%s\n", strerror(errno));
	}
}

int create_sems(){
	key_t key = get_key(SEMAPHORE);
	
	int semid;
	if((semid = semget(key, SHOP_CAP, IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not initialize semaphore array.");
	}

	unsigned short init_val[SHOP_CAP];
	for(int i = 0; i < SHOP_CAP; i++){
		init_val[SHOP_CAP] = 1;
	}

	union semnum arg;
	arg.array = init_val;
	
	if(semctl(semid, 0, SETALL, arg) == -1){
		error("Could not initialize semaphore values.");
	}

	return semid;
}

int* create_array(){
	key_t key = get_key(ARRAY);

	if((arrid = shmget(key, SHOP_CAP * sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not create shared array.");
	}

	Order* arr = (Order*) shmat(arrid, NULL, 0666);
	for(int i = 0; i < SHOP_CAP; i++){
		arr[i].num = 0;
		arr[i].state = -1;
	}

	return arr;
}

Counter* create_counter(){
	key_t key = get_key(COUNTER);

	if((countid = shmget(key, sizeof(Counter), IPC_CREAT | IPC_EXCL | 0666)) == -1){
		error("Could not create shared counter.");
	}

	Counter* counter = (Counter *) shmat(countid, NULL, 0666);
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

    printf("Shop closing.\n");

	raise(SIGINT);

    return 0;
}