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

int find_order_to_send(){
    for(int i = 0; i < SHOP_CAP; i++){
        if(arr[(i + counter->max_id - counter->to_prepare - counter->to_send) % SHOP_CAP].state == 1){
            return (i + counter->max_id - counter->to_prepare - counter->to_send) % SHOP_CAP;
        }
    }

    return -1;
}
int main(){
    get_to_work();
    char buf[84];
    int order_to_send;

    while(1){
        if((order_to_send = find_order_to_send()) != -1){
            if(sem_wait(semid[order_to_send]) == -1){
                error("Could not lock semaphore.");
            }

            arr[order_to_send].num *= 3;
            arr[order_to_send].state = -1;
            counter -> to_send--;

            printf("%d %s Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.\n", getpid(), gettimestamp(buf), arr[order_to_send].num, counter->to_prepare, counter->to_send);

            if(sem_post(semid[order_to_send]) == -1){
                error("Could not unlock semaphore.");
            }
        }

        sleep(1);
    }

    go_home();

    return 0;
}