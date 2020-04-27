#include "settings.h"
#include "shared.h"

int semid;
Order* arr;
Counter* counter;

void go_home(){
    detach(arr);
    detach(counter);
}

void get_to_work(){
    atexit(go_home);
    signal(SIGINT, go_home);
    srand(time(NULL));

    arr = get_array();
    counter = get_counter();
    semid = get_semaphores();
}

int find_free_space(Order* arr, Counter* counter){
    for(int i = 0; i < SHOP_CAP; i++){
        if(arr[(i + counter->to_prepare + counter->to_send) % SHOP_CAP].num == 0){
            return (i + counter->to_prepare + counter->to_send) % SHOP_CAP;
        }
    }

    return -1;
}

int main(){
    get_to_work();

    while(1){
        if(counter -> to_send + counter -> to_prepare < SHOP_CAP){
            int free_space = find_free_space(arr, counter);
            lock_semaphore(semid, free_space);

            arr[free_space].num = (rand() % 10000) + 1;
            arr[free_space].state = 0;
            counter->to_prepare++;

            printf("%d %s Dodałem liczbę: %d. Liczba zamówień do przygotowania: %d. Liczba zamówień do wysłania: %d.\n", getpid(), gettimestamp(), arr[free_space].num, counter->to_prepare, counter->to_send);

            unlock_semaphore(semid, free_space);
        }
        else{
            break;
        }
    }

    go_home();

    return 0;
}