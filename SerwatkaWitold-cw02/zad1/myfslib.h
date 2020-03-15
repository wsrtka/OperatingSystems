#ifndef myfslib
#define myfslib

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

int error(char* message);
double time_difference(clock_t time1, clock_t time2);
void write_result(struct tms* time_start, struct tms* time_end);
void generate_file(char* file_name, int count, int bytes);
char* sys_get_block(int fd, int index, int length);
void sys_write_block(int fd, char* block, int index, int length);
void sys_sort(char* filename, int records, int length);
void sys_copy(char* file1, char* file2, int records, int length);
char* lib_get_block(int fd, int index, int length);
void lib_write_block(int fd, char* block, int index, int length);
void lib_sort(char* filename, int records, int length);
void lib_copy(char* file1, char* file2, int records, int length);

#endif