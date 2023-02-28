/**
 * @file mrush.c
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-02-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "minero.h"

int miner_stat = 1;

int main(int argc, char** argv) {

    pid_t pid;
    int status;

    /* Comprobamos que los argumentos sean correctos */
    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        exit(EXIT_FAILURE);
    }

    /* Lanzamos al minero*/
    pid = fork();
    if(pid < 0) {
        printf("Error creating minero fork\n");
        exit(EXIT_FAILURE);
    }else if(pid == 0) {
        miner_stat = minero(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
        exit(miner_stat);
    }
        
    /* Esperamos a que termine el minero */
    wait(&status);

    /* Comprobamos el estado de salida del minero */
    if(WIFEXITED(status)) {
        miner_stat = WEXITSTATUS(status);
    }else{
        printf("Miner exited unexpectedly\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Miner exited with status %d\n", miner_stat);
    exit(EXIT_SUCCESS);
    
}