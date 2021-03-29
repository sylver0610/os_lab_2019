#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#define DEFAULT_FILENAME "./default_file.txt"

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;
    
    char* file_dir;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = { {"seed", required_argument, 0, 0},
                                          {"array_size", required_argument, 0, 0},
                                          {"pnum", required_argument, 0, 0},
                                          {"by_files", no_argument, 0, 'f'},
                                          {0, 0, 0, 0} };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
        case 0:
            switch (option_index) {
            case 0:
                seed = atoi(optarg);
                // your code here
                // error handling
                if (seed <= 0) {
                    printf("There was an error in 'seed' value");
                    return 1;
                }
                    
                break;
            case 1:
                array_size = atoi(optarg);
                // your code here
                // error handling
                if (array_size <= 0) {
                    printf("There was an error in 'array_size' value");
                    return 1;
                }
                break;
            case 2:
                pnum = atoi(optarg);
                // your code here
                // error handling
                if (pnum <= 0) {
                    printf("There was an error in 'pnum' value");
                    return 1;
                }
                break;
            case 3:
                with_files = true;
                break;

            default:
                printf("Index %d is out of options\n", option_index);
            }
            break;
        case 'f':
            with_files = true;
            break;

        case '?':
            break;

        default:
            printf("getopt returned character code 0%o?\n", c);
        }
    }
    
    // get file name (directory) from argv
    if (with_files) {
        if (optind < argc) {
            file_dir = argv[optind];
            optind++;
        }
        else file_dir = DEFAULT_FILENAME;
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
            argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    
    FILE *f = NULL;
    int p[2];
    
    if (with_files) {
        // open file for writing
        f = fopen(file_dir, "w");
    } else if (pipe(p) < 0) {
        printf("There was an error while generating pipes!\n");
        return 1;
    }
    
    pid_t* pid_list = malloc(sizeof(pid_t) * array_size);

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            // successful fork
            pid_list[active_child_processes] = child_pid;
            active_child_processes += 1;
            if (child_pid == 0) {
                // child process
                
                // parallel somehow

                if (with_files) {
                    // use files here
                    if (f) fprintf(f, "%d\n",array[i]);
                }
                else {
                    // use pipe here
                    write(p[1], &array[i], sizeof(int));
                }
                return 0;
            }
        }
        else {
            printf("Fork failed!\n");
            return 1;
        }
    }
    
    if (f) {
        fclose(f);
        f = NULL;
    }

    while (active_child_processes > 0) {
        // your code here
        kill(pid_list[active_child_processes - 1], SIGKILL);    // kill child process at 'active_child_process' index

        active_child_processes -= 1;
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;
    
    if (with_files) f = fopen(file_dir, "r");
    int value = 0;
    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;
        
        if (with_files) {
            // read from files
            if (f) fscanf(f, "%d", &value);
        }
        else {
            // read from pipes
            read(p[0], &value, sizeof(int));
        }
        
        if (value < min) min = value;
        if (value > max) max = value;

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
    }
    
    if (f)
    {
        fclose(f);
        f = NULL;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}

