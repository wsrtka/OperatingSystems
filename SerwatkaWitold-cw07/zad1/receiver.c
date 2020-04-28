#include "settings.h"
#include "shared.h"

sem_t** semid;
Order* arr;
Counter* counter;

void go_home(){
    detach(semid, SEMS_NAME, SHOP_CAP * sizeof(sem_t*));
    detach(arr, ARR_NAME, SHOP_CAP * sizeof(Order));
    detach(counter, COUNTER_NAME, sizeof(Counter));
}

void get_to_work(){
    atexit(go_home);
    signal(SIGINT, go_home);
    srand(time(NULL));

    arr = attach_array();
    counter = attach_counter();
    semid = get_semaphores();
}

int find_free_space(){
    for(int i = 0; i < SHOP_CAP; i++){
        if(arr[(i + counter->max_id) % SHOP_CAP].state == -1){
            return (i + counter->max_id) % SHOP_CAP;
        }
    }

    return -1;
}

int main(){
    get_to_work();
    char buf[84];
    int free_space;

    while(1){
        if((free_space = find_free_space()) != -1){
            if(sem_wait(semid[free_space]) == -1){
                error("Could not lock semaphore.");
            }

            arr[free_space].num = (rand() % 10000) + 1;
            arr[free_space].state = 0;
            counter->to_prepare++;
            counter->max_id++;

            printf("%d %s Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.\n", getpid(), gettimestamp(buf), arr[free_space].num, counter->to_prepare, counter->to_send);

            if(sem_post(semid[free_space]) == -1){
                error("Could not unlock semaphore.");
            }
        }

        sleep(1);
    }

    go_home();

    return 0;
}