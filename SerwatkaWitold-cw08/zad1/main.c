#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

int* histogram;

typedef struct thread_args {
    int* histogram;
    int from;
    int to;
    int img_width;
    int img_heigth;
    int** img;
} thread_args;

//==========HELPER FUNCTIONS============//
void error(char* msg){
    printf("%s\n", msg);
    printf("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
}

void read_file_header(FILE* file, int* width, int* height, int* maxval){
    rewind(file);

    char* line = NULL;
    char* value = NULL;
    size_t len = 0;

    for(int i = 0; i < 4; i++){
        if(getline(&line, &len, file) == -1){
            error("Could not read header file.");
        }

        switch (i)
        {
        case 0:
            if(strcmp(line, "P2\n") != 0){
                error("Invalid file header.");
            }
            break;

        case 1:
            value = strtok(line, " ");
            value = strtok(NULL, " ");
            printf("Reading file %s", value);
            break;

        case 2:
            value = strtok(line, " ");
            *width = atoi(value);
            value = strtok(NULL, " ");
            *height = atoi(value);
            break;

        case 3:
            *maxval = atoi(line);
            break;
        
        default:
            break;
        }
    }
}

void read_image(FILE* file, int width, int heigth, int** image){
    char* line = NULL;
    char* value = NULL;
    size_t len = 0;
    int i = 0, j = 0;

    while(getline(&line, &len, file) > 0){
        value = strtok(line, " ");

        do{
            image[i++][j] = atoi(value);

            if(i == width){
                i = 0;
                j++;
            }

            if(j == heigth){
                return;
            }
        } while((value = strtok(NULL, " ")) != NULL);
    }
}

struct timeval* time_diff(struct timeval tv1, struct timeval tv2){
    struct timeval* result = malloc(sizeof(struct timeval));

    result->tv_sec = tv1.tv_sec - tv2.tv_sec;
    result->tv_usec = tv1.tv_usec - tv2.tv_usec;

    if(result->tv_usec < 0){
        result->tv_sec--;
        result->tv_usec = -result->tv_usec;
    }

    return result;
}

void print_time(struct timeval* tv_ptr){
    printf("%ld.%ld seconds\n", tv_ptr->tv_sec, tv_ptr->tv_usec);
}

void fprint_time(struct timeval* tv_ptr, FILE* file){
    fprintf(file, "%ld.%ld seconds\n", tv_ptr->tv_sec, tv_ptr->tv_usec);
}

void save_histogram(const char* file_name){
    FILE* result = fopen(file_name, "w");
    if(result == NULL){
        error("Could not create histogram file.");
    }

    for(int i = 0; i < 255; i++){
        fprintf(result, "%d\n", histogram[i]);
    }
    fprintf(result, "%d", histogram[255]);

    fclose(result);
}

//==========SIGN MODE============//
void* sign_function(void* arg){
    struct timeval thread_start;
    gettimeofday(&thread_start, NULL);

    thread_args* args = (thread_args*) arg;

    for(int i = 0; i < args->img_width; i++){

        for(int j = 0; j < args->img_heigth; j++){

            if(args->img[i][j] >= args->from && args->img[i][j] < args->to){
                args->histogram[args->img[i][j]]++;
            }

        }

    }

    struct timeval thread_end;
    gettimeofday(&thread_end, NULL);

    struct timeval* result = time_diff(thread_end, thread_start);

    pthread_exit(result);
}

void thread_sign(const int thread_number, thread_args* args, pthread_t* threads){
    int range = 256 / thread_number;

    thread_args* final[thread_number];

    for(int i = 0; i < thread_number; i++){
        final[i] = malloc(sizeof(thread_args));

        final[i]->from = i * range;
        final[i]->to = (i+1) * range;

        if(i == thread_number - 1 && args->to < 256){
            final[i]->to = 256;
        }

        final[i]->histogram = args->histogram;
        final[i]->img = args->img;
        final[i]->img_heigth = args->img_heigth;
        final[i]->img_width = args->img_width;

        if(pthread_create(&threads[i], NULL, sign_function, (void *) final[i]) != 0){
            error("Could not create thread.");
        }
    }
}

//==========BLOCK MODE============//
void* block_function(void* arg){
    struct timeval thread_start;
    gettimeofday(&thread_start, NULL);

    thread_args* args = (thread_args*) arg;

    for(int i = args->from; i < args->to; i++){

        for(int j = 0; j < args->img_heigth; j++){
            args->histogram[args->img[i][j]]++;
        }

    }

    struct timeval thread_end;
    gettimeofday(&thread_end, NULL);

    struct timeval* result = time_diff(thread_end, thread_start);

    pthread_exit(result);
}

void thread_block(const int thread_number, thread_args* args, pthread_t* threads){
    int range = args->img_width / thread_number;

    thread_args* final[thread_number];

    for(int i = 0; i < thread_number; i++){
        final[i] = malloc(sizeof(thread_args));

        final[i]->from = i * range;
        final[i]->to = (i+1) * range;

        if(i == thread_number - 1 && final[i]->to <= args->img_width){
            final[i]->to = args->img_width;
        }

        final[i]->histogram = args->histogram;
        final[i]->img = args->img;
        final[i]->img_heigth = args->img_heigth;
        final[i]->img_width = args->img_width;

        if(pthread_create(&threads[i], NULL, block_function, (void *) final[i]) != 0){
            error("Could not create thread.");
        }
    }
}

//==========INTERLEAVED MODE============//
void* interleaved_function(void* arg){
    struct timeval thread_start;
    gettimeofday(&thread_start, NULL);

    thread_args* args = (thread_args*) arg;

    for(int i = args->from; i < args->img_width; i += args->to){

        for(int j = 0; j < args->img_heigth; j++){

            args->histogram[args->img[i][j]]++;

        }

    }

    struct timeval thread_end;
    gettimeofday(&thread_end, NULL);

    struct timeval* result = time_diff(thread_end, thread_start);

    pthread_exit(result);
}

void thread_interleaved(const int thread_number, thread_args* args, pthread_t* threads){
    thread_args* final[thread_number];

    for(int i = 0; i < thread_number; i++){
        final[i] = malloc(sizeof(thread_args));

        final[i]->from = i;
        final[i]->to = thread_number;
        final[i]->histogram = args->histogram;
        final[i]->img = args->img;
        final[i]->img_heigth = args->img_heigth;
        final[i]->img_width = args->img_width;

        if(pthread_create(&threads[i], NULL, interleaved_function, (void *) final[i]) != 0){
            error("Could not create thread.");
        }
    }
}

//==========MAIN============//
int main(int argc, char* argv[]){
    //kontrola argumentów
    if(argc != 5){
        error("Invalid number of arguments.");
    }

    //wczytanie argumentow
    const int THREADS_NO = atoi(argv[1]);
    const char* MODE = argv[2];
    const char* IN_FILE = argv[3];
    const char* OUT_FILE = argv[4];

    //wczytanie metadanych
    FILE* input_file = fopen(IN_FILE, "r");
    if(input_file == NULL){
        error("Could not open input file.");
    }

    int width, height, maxval;

    read_file_header(input_file, &width, &height, &maxval);

    if(width < 1 || height < 1 || maxval < 0){
        error("Invalid image metadata.");
    }

    histogram = calloc(maxval + 1, sizeof(int));

    //ustawianie argumentów dla funkcji threads
    thread_args* args = malloc(sizeof(args));
    args->histogram = histogram;
    args->img_heigth = height;
    args->img_width = width;

    args->img = malloc(width * sizeof(int*));
    for(int i = 0; i < width; i++){
        args->img[i] = malloc(height * sizeof(int));
    }

    read_image(input_file, width, height, args->img);

    fclose(input_file);

    //wątkowanie
    pthread_t threads[THREADS_NO];
    
    struct timeval main_start;
    gettimeofday(&main_start, NULL);

    if(strcmp(MODE, "sign") == 0){
        thread_sign(THREADS_NO, args, threads);
    }
    else if(strcmp(MODE, "block") == 0){
        thread_block(THREADS_NO, args, threads);
    }
    else if(strcmp(MODE, "interleaved") == 0){
        thread_interleaved(THREADS_NO, args, threads);
    }
    else{
        error("Invalid mode.");
    }

    //podsumowanie
    void* retrval;
    FILE* times_file = fopen("Times.txt", "a");
    if(times_file == NULL){
        error("Could not open Times.txt.");
    }

    for(int i = 0; i < THREADS_NO; i++){
        if(pthread_join(threads[i], &retrval) == 0){
            printf("Thread %ld worked for ", threads[i]);
            print_time(retrval);
            fprintf(times_file, "Thread %ld worked for ", threads[i]);
            fprint_time(retrval, times_file);
        }
    }

    struct timeval main_end;
    gettimeofday(&main_end, NULL);

    printf("Main function finished after ");
    print_time(time_diff(main_end, main_start));
    fprintf(times_file, "Main function finished after ");
    fprint_time(time_diff(main_end, main_start), times_file);

    save_histogram(OUT_FILE);

    return 0;
}