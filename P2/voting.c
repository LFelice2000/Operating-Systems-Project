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
#include <fcntl.h>
#include "minero.h"

int main(int argc, char *argv[]) {
    int nprocs = 0, i;
    FILE *fpid = NULL;
    pid_t pid;
    char bufpid[30] = "\0";


    if(argc < 3) {
        printf("Error, debes iniciar el programa como: ./voting <N_PROCS> <N_SECS>.\n");
        exit(EXIT_FAILURE);
    }
    
    
    nprocs = atoi(argv[1]);

    i = 0;
    while ((pid = fork()) != 0 && i <= nprocs)
    {
        if(pid) {
            fpid = fopen("pids.txt", "a+");
            if(fpid == NULL) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            sprintf(bufpid, "Proceso %d: %d\n", (i+1), pid);
            fwrite(bufpid, strlen(bufpid), 1, fpid);
            fflush(fpid);
            fclose(fpid);

            i++;
        }else if(pid == 0) {
            printf("Proceso hijo esperando...\n");
            
        }
    }
    

   
    while (wait(NULL) != -1);
    
    exit(EXIT_SUCCESS);
}