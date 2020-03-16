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
    int option;

    const char* short_options = "mad";

    const struct option long_options[] = {
        { "mtime", 1, NULL, 'm' },
        { "atime", 1, NULL, 'a' },
        { "maxdepth", 1, NULL, 'd'},
        { NULL, 0, NULL, 0}
    };

    do{
        option = getopt_long(argc, argv, short_options, long_options, NULL);

        switch (option)
        {
        case 'm':
            /* code */
            break;

        case 'a':
            /* code */
            break;

        case 'd':
            /* code */
            break;

        case -1:
            /* code */
            break;
        
        default:
            abort();
        }
    }while(option != -1);

    return 0;
}