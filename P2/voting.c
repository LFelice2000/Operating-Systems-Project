/**
 * @file voting.c
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-02-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include "votante.h"

int got_signal = 0;

void handler(int sig) {
    printf("Señal %d recibida\n", sig);
    got_signal = 1;
}

int main(int argc, char *argv[]) {

    FILE *fpid = NULL;
    pid_t pid;
    char bufpid[30] = "\0";
    int nprocs = 0, i, *pids = NULL, semval = 0;
    sem_t *sem;

    struct sigaction act;

    if(argc < 3) {
        printf("Error, debes iniciar el programa como: ./voting <N_PROCS> <N_SECS>\n");
        exit(EXIT_FAILURE);
    }
    
    nprocs = atoi(argv[1]);
    pids = (int *)malloc(nprocs * sizeof(int));

    /* Se configura la captura de señales */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Se crea el semáforo */
    sem = sem_open("candsem", O_CREAT, 0644, 1);
    if(sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /* Se verifica si el semáforo está en 0 */
    sem_getvalue(sem, &semval);
    if(semval == 0) {
        sem_post(sem);
    }

    /* Se crean los procesos votantes */
    i = 0;
    while (i < nprocs){
        pid = fork();
        if(pid) {

            /* Se guarda el pid del proceso votante */
            pids[i] = pid;

            fpid = fopen("pids.txt", "a+");
            if(fpid == NULL) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            sprintf(bufpid, "%d\n", pid);
            fwrite(bufpid, strlen(bufpid), 1, fpid);
            fflush(fpid);
            fclose(fpid);

        } else if(pid == 0) {
            /* Se ejecuta el proceso votante */
            votante(nprocs);
            exit(EXIT_SUCCESS);
        }

        i++;
    }

    /* El proceso Principal envía la señal SIGUSR1 a todos los procesos votantes */
    for (i = 0; i < nprocs; i++){
        printf("Enviando señal SIGUSR1 al proceso %d\n", pids[i]);
        kill(pids[i], SIGUSR1);
    }

    sigsuspend(&act.sa_mask);

    /* Si el proceso Principal ha recibido SIGINT, envía SIGTERM a todos los procesos votantes */
    if(got_signal){
        for (i = 0; i < nprocs; i++){
            kill(pids[i], SIGTERM);
        }
        printf("Finishing by signal\n");
    }

    /* Se recogen a los procesos votantes */
    for (i = 0; i < nprocs; i++){
        waitpid(pids[i], NULL, 0);
    }

    /* Se destruye el semáforo */
    if(sem_destroy(sem) == -1) {
        perror("sem_destroy");
        exit(EXIT_FAILURE);
    }

    /* Se libera la memoria */
    free(pids);
    
    exit(EXIT_SUCCESS);
}