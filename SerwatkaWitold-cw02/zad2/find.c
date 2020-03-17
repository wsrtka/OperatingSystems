#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <linux/limits.h>

//TODO:
//set EXIT_FAILURE and EXIT_SUCCESS in error() calls
//remove all callocs where there should be none

void error(int exit_code){
    printf("Usage: find <directory> <options>\n");
    printf("-mtime      Find file modified at least n+1 days ago.\n-atime      Find file accessed at least n+1 days ago.\n-maxdepth   Descend at most n levels below the starting-points.\n");
    exit(exit_code);
}

char* get_filepath(struct dirent* file, char* dir_name){
    const char* file_path;
    strcpy(file_path, dir_name);
    strcat(file_path, "/");
    strcat(file_path, file->d_name);

    return file_path;
}

void print_abs_filepath(struct dirent* file, char* dir_name){
    const char* file_path = get_filepath(file, dir_name);

    char actual_path[PATH_MAX];
    char* path;

    path = realpath(file_path, actual_path);

    if(path == NULL){
        error(EXIT_FAILURE);
    }

    printf("Directory contains file %s found. Data:\n Absolute path: %s\n", file->d_name, path);
}

void print_file_info(struct dirent* file, char* dir_name, struct stat file_stats){
    const char* file_path = get_filepath(file, dir_name);

    switch (file_stats.st_mode & __S_IFMT)
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

    printf("Link count:               %ld\n", (long) file_stats.st_nlink);

    printf("File size:                %lld bytes\n", (long long) file_stats.st_size);

    printf("Last file access:         %s\n", ctime(&file_stats.st_atime));
    printf("Last file modification:   %s\n\n", ctime(&file_stats.st_mtime));
}

void search_directory(DIR* dir, char* dir_name, int mtime, int atime, int depth){
    rewinddir(dir);
    struct dirent* current_file = calloc(1, sizeof(struct dirent)); //apparently tego nie trzeba (nawet nie wolno) free()? wg. linux.die.net
    struct stat file_stats;

    do{
        //TODO: check atime, dtime, depth etc.
        current_file = readdir(dir);

        if(stat(get_filepath(current_file, dir_name), &file_stats) == -1){
            error(EXIT_FAILURE);
        }

        if(difftime(file_stats.st_mtime, time(NULL))/86400 > mtime){ //bez mtime != -1 bo warunek na dodatniość powinien być zachowany
            continue;
        }

        if(difftime(file_stats.st_atime, time(NULL))/86400 > atime){ //analogicznie jak wyżej
            continue;
        }
        
        print_abs_filepath(current_file, dir_name);
        print_file_info(current_file, dir_name, file_stats);

        if((file_stats.st_mode & __S_IFMT) == __S_IFDIR && depth > 0){
            char* current_name = get_filepath(current_file, dir_name);
            DIR* new_dir = opendir(current_name);

            search_directory(new_dir, current_name, mtime, atime, depth-1);
        }
    }while(current_file != NULL);
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

    int mtime = -1, atime = -1, depth = -1;
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

    DIR* directory = opendir(dir_name);
    if(directory == NULL){
        error(0);
    }

    search_directory(directory, dir_name, mtime, atime, depth);

    return 0;
}