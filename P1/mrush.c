#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char** argv) {
    pid_t pid;
    int ret;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        return EXIT_FAILURE;
    }

    /* Parse the input values */
    
    printf("EL proceso principal lanza minero...\n");
    
    /* Lanzamos al minero*/
    pid = fork();
    if(pid < 0) {
        printf("Error creating minero fork\n");
        exit(EXIT_FAILURE);
    }
    if(pid == 0) {
        ret = execvp("./minero", argv);
        if(ret == EXIT_FAILURE) {
            printf("Mier exited unexpectedly\n");
            exit(EXIT_FAILURE);
        }

        printf("Miner exited with status %d\n", ret);
    }
    
    wait(NULL);
    exit(EXIT_SUCCESS);
}