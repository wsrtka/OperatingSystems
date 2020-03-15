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

void generate_file(char* file_name, int count, int bytes);
char* sys_get_block(int fd, int index, int length);
void sys_write_block(int fd, char* block, int index, int length);
void sys_sort(char* filename, int records, int length);
void sys_copy(char* file1, char* file2, int records, int length);
char* lib_get_block(int fd, int index, int length);
void lib_write_block(int fd, char* block, int index, int length);
void lib_sort(char* filename, int records, int length);
void lib_copy(char* file1, char* file2, int records, int length);