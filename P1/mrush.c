#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char** argv) {
    pid_t pid;
    int ret, miner_stat;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        exit(EXIT_FAILURE);
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
            printf("Error in miner execution\n");
            exit(EXIT_FAILURE);
        }
    }
    
    miner_stat = wait(NULL);
    if(miner_stat < 0) {
       printf("Miner exited unexpectedly\n");
       
       exit(EXIT_FAILURE);
    } else {
        printf("Miner exited with status %d\n", miner_stat);
    }

    exit(EXIT_SUCCESS);
}