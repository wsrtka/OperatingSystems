#include "myfslib.h"

#define size 128

int error(char* message){
    fprintf(stderr, message);
    exit(1);
}

double time_difference(clock_t time1, clock_t time2){
    return (double)(time2 - time1) / sysconf(_SC_CLK_TCK);
}

void write_result(struct tms* time_start, struct tms* time_end){
    printf("User time: %fl\n", timeDifference(time_start->tms_utime, time_end->tms_utime));
    printf("System time: %fl\n", timeDifference(time_start->tms_stime, time_end->tms_stime));

    // fprintf(result_file, "User time: %fl\n", timeDifference(time_start->tms_utime, time_end->tms_utime));
    // fprintf(result_file, "System time: %fl\n", timeDifference(time_start->tms_stime, time_end->tms_stime));
}

void generate_file(char* file_name, int count, int bytes){
    char* command = calloc(size, sizeof(char));

    strcpy(command, "head /dev/urandom | tr -dc A-Za-z0-9 | head -c ");
    strcpy(command, bytes);
    strcpy(command, " | fold -w ");
    strcpy(command, count);
    strcpy(command, " > ");
    strcpy(command, file_name);

    system(command); 

    free(command);   
}

char* sys_get_block(int fd, int index, int length){
    if(lseek(fd, index*length, SEEK_SET) == -1){
        error("Could not set position in sys_get_block");
    }

   char* block = calloc(length, sizeof(char));

   if(read(fd, block, length) == 0){
       error("Could not read block in sys_get_block");
   } 

   return block;
}

void sys_write_block(int fd, char* block, int index, int length){
    if(lseek(fd, index*length, SEEK_SET) == -1){
        error("Could not set position in sys_write_block");
    }

    if(write(fd, block, length) == 0){
        error("Could not write to file in sys_write_block");
    }
}

int sys_partition(int fd, int from, int to, int length){
    char* pivot = calloc(length, sizeof(char));
    strcpy(pivot, sys_get_block(fd, to, length));

    int i = from;

    for(int j = from; j <= to; j++){
        if(strcmp(sys_get_block(fd, j, length), pivot) < 0){
            char* block = calloc(length, sizeof(char));
            strcpy(block, sys_get_block(fd, j, length));

            sys_write_block(fd, sys_get_block(fd, i, length), j, length);
            sys_write_block(fd, block, i++, length);

            free(block);
        }
    }

    free(pivot);
}

void sys_qsort(int fd, int from, int to, int length){
    if(from > to){
        return;
    }

    int q = sys_partition(fd, from, to, length);

    sys_qsort(fd, from, q-1, length);
    sys_qsort(fd, q+1, to, length);
}

void sys_copy(int fd1, int fd2, int records, int length){
    char* buffor = calloc(length, sizeof(char));
    lseek(fd2, 0, SEEK_END);

    for(int i = 0; i < records; i++){
        strcpy(buffor, sys_get_block(fd1, i, length));

        if(write(fd2, buffor, length) < 1){
            error("Unable to write to file in sys_copy");
        }
    }
}

char* lib_get_block(FILE* file, int index, int length){
    if(fseek(file, index*length, 0) != 0){
        error("Could not set file position in lib_get_block");
    }

    char* block = calloc(length, sizeof(char));

    if(fread(block, length, length, file) == 0){
        error("Could not read file in lib_get_block");
    }

    return block;
}

void lib_write_block(FILE* file, char* block, int index, int length){
    if(fseek(file, index*length, 0) != 0){
        error("Could not set file position in lib_write_block");
    }

    if(fwrite(block, length, length, file) != length){
        error("Could not write file in lib_write_block");
    }
}

void lib_qsort(FILE* file, int records, int length);
void lib_copy(FILE* file1, FILE* file2, int records, int length);