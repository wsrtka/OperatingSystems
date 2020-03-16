#ifndef myfslib
#define myfslib

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
void sys_qsort(int fd, int from, int to, int length);
void sys_copy(int fd1, int fd2, int records, int length);
char* lib_get_block(FILE* file, int index, int length);
void lib_write_block(FILE* file, char* block, int index, int length);
void lib_qsort(FILE* file, int from, int to, int length);
void lib_copy(FILE* file1, FILE* file2, int records, int length);

#endif