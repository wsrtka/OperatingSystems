#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <linux/limits.h>

int depth = -1;

void error(int exit_code){
    printf("Usage: find <directory> <options>\n");
    printf("-mtime      Find file modified at least n+1 days ago.\n-atime      Find file accessed at least n+1 days ago.\n-maxdepth   Descend at most n levels below the starting-points.\n");
    exit(exit_code);
}

void print_abs_filepath(const char* file_path){
    char actual_path[PATH_MAX];
    char* path;

    path = realpath(file_path, actual_path);

    if(path == NULL){
        error(EXIT_FAILURE);
    }

    printf("Directory contains file %s. Data:\n Absolute path: %s\n", basename(path), path);
}

void print_file_info(const char* fpath, struct stat *file_stats){

    switch (file_stats->st_mode & __S_IFMT)
    {
    case __S_IFBLK:  printf("block device\n");            break;
    case __S_IFCHR:  printf("character device\n");        break;
    case __S_IFDIR:  printf("directory\n");               break;
    case __S_IFIFO:  printf("FIFO/pipe\n");               break;
    case __S_IFLNK:  printf("symlink\n");                 break;
    case __S_IFREG:  printf("regular file\n");            break;
    case __S_IFSOCK: printf("socket\n");                  break;
    default:         printf("unknown?\n");                break;
    }

    printf("Link count:               %ld\n", (long) file_stats->st_nlink);

    printf("File size:                %lld bytes\n", (long long) file_stats->st_size);

    printf("Last file access:         %s\n", ctime(&file_stats->st_atime));
    printf("Last file modification:   %s\n\n", ctime(&file_stats->st_mtime));
}

void search_directory(const char* fpath, const struct stat *fstats, int tflag, struct FTW* ftwbuf){
    if(ftwbuf->level > depth){
        return 0;
    }

    print_abs_filepath(fpath);
    print_file_info(fpath, fstats);

    return 0;
}

int main(int argc, char** argv){
    int next_option;

    const char* short_options = "m:a:d:";

    const struct option long_options[] = {
        { "mtime", 1, NULL, 'm' },
        { "atime", 1, NULL, 'a' },
        { "maxdepth", 1, NULL, 'd'},
        { NULL, 0, NULL, 0}
    };

    char* dir_name = calloc(strlen(argv[1]), sizeof(char));
    strcpy(dir_name, argv[1]);

    int mtime = -1, atime = -1;
    int args_passed = 0;

    optind = 2;

    do{
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);

        switch (next_option)
        {
        case 'm':
            mtime = atoi(optarg[args_passed]);
            args_passed++;
            break;

        case 'a':
            atime = atoi(optarg[args_passed]);
            args_passed++;
            break;

        case 'd':
            depth = atoi(optarg[args_passed]);
            args_passed++;
            break;

        case -1:
            break;
        
        default:
            abort();
        }
    }while(next_option != -1);

    int flags = 0;
    flags |= FTW_PHYS;


    if(nftw(dir_name, search_directory, 20, flags))

    return 0;
}