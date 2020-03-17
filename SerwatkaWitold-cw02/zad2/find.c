#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

void error(int exit_code){
    printf("Usage: find <directory> <options>\n");
    printf("-mtime      Find file modified at least n+1 days ago.\n-atime      Find file accessed at least n+1 days ago.\n-maxdepth   Descend at most n levels below the starting-points.\n");
    exit(exit_code);
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

    char* search_dir = calloc(strlen(argv[1]), sizeof(char));
    strcpy(search_dir, argv[1]);

    int mtime, atime, depth;
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

    return 0;
}