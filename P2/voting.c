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
    if(sig == SIGINT) {
        got_signal = 1;
    }else if(sig == SIGALRM) {
        got_signal = 2;
    }
}

int main(int argc, char *argv[]) {

    FILE *fpid = NULL;
    pid_t pid, *pids = NULL;
    char bufpid[30] = "\0";
    int nprocs = 0, i, semval = 0;
    sem_t *candsem = NULL, *votsem = NULL;
    sigset_t set, oldset;

    struct sigaction act;

    if(argc < 3) {
        printf("Error, debes iniciar el programa como: ./voting <N_PROCS> <N_SECS>\n");
        exit(EXIT_FAILURE);
    }
    
    nprocs = atoi(argv[1]);
    pids = (pid_t *) calloc (nprocs, sizeof(pid_t));
    if(pids == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Se configura la captura de señales */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    /* Se bloquean las señales */
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGALRM);

    sigprocmask(SIG_BLOCK, &set, &oldset);

    if(sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGALRM, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if(alarm(atoi(argv[2])) < 0) {
        perror("alarm");
        exit(EXIT_FAILURE);
    }

    /* Se crea el semáforo para elegir candidato */
    candsem = sem_open("candsem", O_CREAT, 0644, 1);
    if(candsem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /* Se verifica si el semáforo está en 0 */
    sem_getvalue(candsem, &semval);
    if(semval == 0) {
        sem_post(candsem);
    }

    /* Se crea el semáforo para votar */
    votsem = sem_open("votsem", O_CREAT, 0644, 1);
    if(votsem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /* Se verifica si el semáforo está en 0 */
    sem_getvalue(votsem, &semval);
    if(semval == 0) {
        sem_post(votsem);
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
                sem_close(candsem);
                sem_close(votsem);
                sem_unlink("candsem");
                sem_unlink("votsem");
                exit(EXIT_FAILURE);
            }

            sprintf(bufpid, "%d\n", pid);
            fwrite(bufpid, strlen(bufpid), 1, fpid);
            fflush(fpid);
            fclose(fpid);

        } else if(pid == 0) {
            /* Cada proceso hijo ejecuta la función competicion */
            competicion(nprocs);
        }

        i++;
    }

    /* El proceso Principal envía la señal SIGUSR1 a todos los procesos votantes */
    for (i = 0; i < nprocs; i++){
        kill(pids[i], SIGUSR1);
    }

    sigsuspend(&oldset);

    /* Si el proceso Principal ha recibido SIGINT o SIGALRM, envía SIGTERM a todos los procesos votantes */
    if(got_signal){
        for (i = 0; i < nprocs; i++){
            kill(pids[i], SIGTERM);
        }
        if(got_signal == 1){
            printf("Finishing by signal\n");
        }else if(got_signal == 2){
            printf("Finishing by alarm\n");
        }
    }

    /* Se recogen a los procesos votantes */
    for (i = 0; i < nprocs; i++){
        waitpid(pids[i], NULL, 0);
    }

    /* Se cierran los semáforos */
    sem_close(candsem);
    sem_close(votsem);

    /* Se eliminan los semáforos */
    sem_unlink("candsem");
    sem_unlink("votsem");

    /* Se libera la memoria */
    free(pids);
    
    exit(EXIT_SUCCESS);
}

void setup(){
    
}