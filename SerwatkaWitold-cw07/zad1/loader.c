#include "settings.h"
#include "shared.h"

int semid;
Order* arr;
Counter* counter;

void get_to_work(){
    atexit(go_home);
    signal(SIGINT, go_home);
    srand(time(NULL));

    arr = attach_array();
    counter = attach_counter();
    semid = get_semaphores();
}

int find_order_to_load(){
    for(int i = 0; i < SHOP_CAP; i++){
        if(arr[(i + counter->max_id - counter->to_send - counter->to_send) % SHOP_CAP].state == 0){
            return (i + counter->max_id - counter->to_send - counter->to_send) % SHOP_CAP;
        }
    }

    return -1;
}

int main(){
    get_to_work();
    char buf[84];
    int order_to_load;

    while(1){
        if((order_to_load = find_order_to_load()) != -1){
            lock_semaphore(semid, order_to_load);

            arr[order_to_load].num *= 2;
            arr[order_to_load].state == 1;
            counter->to_prepare--;
            counter->to_send++;

            printf("%d %s Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.\n", getpid(), gettimestamp(buf), arr[order_to_load].num, counter->to_prepare, counter->to_send);

            unlock_semaphore(semid, order_to_load);
        }
    }

    go_home();

    return 0;
}