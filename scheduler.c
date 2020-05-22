#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scheduler.h"


int main(int argc, char *argv[]) {

    // initialise all the variables
    char *file_name;  // the file name
    int sche_algo;    // indicate what scheduling algorithm is being used

    int c;

    while ((c = getopt(argc, argv, "f:a:m:s:q:"))) {
        int getopt_ended = 0;
        switch (c) {
            case 'f':
                file_name = (char*)malloc(sizeof(char) * MAX_FILENAME);
                sprintf(file_name, "%s", optarg);
                printf("file name: %s\n", file_name);
                break;
            case 'a':
                printf("a flag found\n");
                break;
            case 'm':
                printf("m flag found\n");
                break;
            case 's':
                printf("s flag found\n");
                break;
            case 'q':
                printf("q flag found\n");
                break;
            default:
                getopt_ended = 1;  // finished reading the flags
        }

        if (getopt_ended) break;

    }

    free(file_name);

    return 0;
}

