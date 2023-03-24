/**
 * @file votante.c
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-03-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include "votante.h"

int got_SIGUSR1 = 0;
int got_SIGUSR2 = 0;

pid_t *pids = NULL;
sem_t *candsem = NULL;
sem_t *votsem = NULL;

sigset_t set, oldset, termset;
struct sigaction act;

void sighandler(int sig) {

    if(sig == SIGUSR1){
        got_SIGUSR1 = 1;
    }else if(sig == SIGUSR2){
        got_SIGUSR2 = 1;
    }else if(sig == SIGTERM){
        free(pids);
        sem_close(candsem);
        sem_close(votsem);
        exit(EXIT_SUCCESS);
    }

}

void competicion(int nprocs, sem_t *csem, sem_t *vsem) {

    FILE *fp = NULL;
    char bufpid[30] = "\0";
    pid_t pid;
    int i;

    /* se reserva memoria para los pids */
    pids = (pid_t *) malloc((nprocs-1) * sizeof(pid_t));
    if(pids == NULL) {
        perror("malloc");
        exit(1);
    }

    /* configurar señales */
    señales();

    /* configurar semaforos */
    candsem = csem;
    votsem = vsem;

    /* esperar a que el sistema esté listo y reciba SIGUSR1 */
    while(got_SIGUSR1 == 0) {
        sigsuspend(&oldset);
    }
    
    /* abrir fichero para leer los pids */
    fp = fopen("pids.txt", "r");
    if(fp == NULL) {
        perror("fopen");
        exit(1);
    }

    /* leer los pids de los procesos candidatos */
    i = 0;
    while(fscanf(fp, "%d", &pid) != EOF && i < nprocs){
        /* si el pid es distinto del mio, lo guardo */
        if(pid != getpid()) {
            pids[i] = pid;
            i++;
        }
    }
    fclose(fp);

    /* Rondas de votación */
    while(got_SIGUSR1) {

        got_SIGUSR1 = 0;

        /* decidir si soy candidato o no */
        if(sem_trywait(candsem) == 0) {
            candidato(nprocs, candsem, votsem);
        }else{
            votante(votsem);
        }

    }

}

void señales(){

    /* bloquear señales */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    sigprocmask(SIG_BLOCK, &set, &oldset);

    /* bloquear señal SIGTERM */
    sigemptyset(&termset);
    sigaddset(&termset, SIGTERM);

    /* configurar la captura de señales */
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGUSR2, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGTERM, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    return;
}

void candidato(int nprocs, sem_t *candsem, sem_t *votsem){

    FILE *fp = NULL;
    int voto = 0, i = 0, yes = 0, no = 0;

    /* bloquear señal SIGTERM */
    sigprocmask(SIG_BLOCK, &termset, NULL);

    /* abrir fichero donde se guardan los votos */
    fp = fopen("votos.txt", "w");
    if(fp == NULL) {
        perror("fopen");
        return;
    }
    fclose(fp);

    /* desbloquear señal SIGTERM */
    sigprocmask(SIG_UNBLOCK, &termset, NULL);

    /* enviar señal SIGUSR2 a los demás procesos */
    for(i = 0; i < nprocs-1; i++) {
        kill(pids[i], SIGUSR2);
    }

    while(1){

        /* se accede a la sección crítica */
        sem_wait(votsem);

        sigprocmask(SIG_BLOCK, &termset, NULL);

        /* abrir fichero para leer los votos */
        fp = fopen("votos.txt", "r");
        if(fp == NULL) {
            perror("fopen");
            return;
        }

        /* comprobar si todos los votos han sido contabilizados */
        fseek(fp, 0, SEEK_END);

        if(ftell(fp) == ((nprocs-1)*2)){
            rewind(fp);
            fclose(fp);

            /* todos los votos han sido contabilizados */
            fprintf(stdout, "Candidate %d => [ ", getpid());
            fp = fopen("votos.txt", "r");
            if(fp == NULL) {
                perror("fopen");
                return;
            }

            /* resetear variables */
            yes = 0;
            no = 0;

            /* leer votos */
            while(!feof(fp)) {
                fscanf(fp, "%d\n", &voto);
                if(voto == 0) {
                    fprintf(stdout, "N ");
                    no++;
                } else {
                    fprintf(stdout, "Y ");
                    yes++;
                }
            }

            if(yes > no) {
                fprintf(stdout, "] => Accepted\n");
            } else {
                fprintf(stdout, "] => Rejected\n");
            }

            fclose(fp);
            sem_post(votsem);
            sem_post(candsem);

            sigprocmask(SIG_UNBLOCK, &termset, NULL);

            /* enviar señal SIGUSR1 a los demás procesos */
            for(i = 0; i < nprocs-1; i++) {
                kill(pids[i], SIGUSR1);
            }

            /* empieza la siguiente ronda */
            got_SIGUSR1 = 1;
            usleep(250000);

            break;
        }

        rewind(fp);
        fclose(fp);

        sigprocmask(SIG_UNBLOCK, &termset, NULL);

        /* no todos los votos han sido contabilizados */
        /* se sale de la sección crítica */
        sem_post(votsem);
        usleep(10000);
    }

    return;
}

void votante(sem_t *votsem){

    FILE *fp = NULL;
    int voto = 0;

    /* esperar a recibir SIGUSR2 */
    while(got_SIGUSR2 == 0) {
        sigsuspend(&oldset);
    }

    /* zona crítica de escritura */
    sem_wait(votsem);

    /* bloquear SIGTERM mientras escribo en el fichero */
    sigprocmask(SIG_BLOCK, &termset, NULL);

    /* abrir fichero para escribir los votos */
    fp = fopen("votos.txt", "a+");
    if(fp == NULL) {
        perror("fopen");
        return;
    }

    /* decidir voto aleatoriamente entre 0 y 1 */
    voto = rand() % 2;

    /* escribir voto en el fichero */
    fprintf(fp, "%d\n", voto);
    fclose(fp);
    sem_post(votsem);
    
    /* termina la zona crítica */
    got_SIGUSR2 = 0;

    /* desbloquear SIGTERM */
    sigprocmask(SIG_UNBLOCK, &termset, NULL);

    /* esperar a recibir SIGUSR1 */
    while(got_SIGUSR1 == 0) {
        sigsuspend(&oldset);
    }

    return;

}