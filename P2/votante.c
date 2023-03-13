#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

int got_SIGUSR1 = 0;
int got_SIGUSR2 = 0;
int got_SIGTERM = 0;

void sighandler(int sig) {
    if(sig == SIGUSR1){
        got_SIGUSR1 = 1;
    }else if(sig == SIGUSR2){
        got_SIGUSR2 = 1;
    }else if(sig == SIGTERM){
        got_SIGTERM = 1;
    }
}

int votante(int nprocs, sem_t *candsem) {

    FILE *fp = NULL;
    struct sigaction act;
    pid_t *pids = NULL, pid;
    char bufpid[30] = "\0";
    int voto = 0, i = 0, yes = 0, no = 0;
    sigset_t oldset;

    pids = (pid_t *) malloc((nprocs) * sizeof(pid_t));
    if(pids == NULL) {
        perror("malloc");
        exit(1);
    }

    /* Se configura la captura de señales */
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);
    sigaddset(&act.sa_mask, SIGUSR2);
    sigaddset(&act.sa_mask, SIGTERM);
    act.sa_flags = 0;
    
    /* bloquear señales */
    sigprocmask(SIG_BLOCK, &act.sa_mask, &oldset);

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

    /* esperar a que el sistema esté listo y se reciba SIGUSR1 */
    sigsuspend(&oldset);

    /* cuando el sistema esté listo */
    while(got_SIGUSR1) {

        got_SIGUSR1 = 0;

        /* abrir fichero para leer los pids */
        fp = fopen("pids.txt", "r");
        if(fp == NULL) {
            perror("fopen");
            exit(1);
        }

        /* leer información del fichero para saber los pids */
        i = 0;
        while(fgets(bufpid, 30, fp) != NULL) {
            pid = atoi(bufpid);
            /* si el pid es distinto del mio, lo guardo */
            if(pid != getpid()) {
                pids[i] = pid;
                i++;
            }
        }
        fclose(fp);

        // this is not working !
        if(sem_trywait(candsem) == -1 && errno == EAGAIN) {

            printf("I'm voter %d\n", getpid());

            sigsuspend(&oldset);

            /* no soy candidato */
            if(got_SIGUSR2) {
                /* abrir fichero para escribir los votos */
                fp = fopen("votos.txt", "a");
                if(fp == NULL) {
                    perror("fopen");
                    return EXIT_FAILURE;
                }
                /* decidir voto aleatorio */
                voto = rand() % 2;
                /* escribir voto en el fichero */
                fprintf(fp, "%d\n", voto);
                fclose(fp);

                got_SIGUSR2 = 0;
            }

            /* espera hasta la siguiente ronda */
            sigsuspend(&oldset);
            

        } else {

            /* soy candidato */
            printf("I'm candidate %d\n", getpid());

            /* abrir fichero para comprobar los votos */
            fp = fopen("votos.txt", "w+");
            if(fp == NULL) {
                perror("fopen");
                return EXIT_FAILURE;
            }

            /* enviar señal SIGUSR2 a los demás procesos */
            for(i = 0; i < nprocs; i++) {
                printf("pid: %d => %d\n", getpid(), pids[i]);
                kill(pids[i], SIGUSR2);
            }

            /* comprobar que todos han votado */
            while(1) {
                fseek(fp, 0, SEEK_END);
                if(ftell(fp) == nprocs) {
                    rewind(fp);
                    break;
                }
                rewind(fp);
                sleep(1000);
            }

            fprintf(stdout, "Candidate %d => [ ", getpid());

            /* leer votos */
            while(!feof(fp)) {
                fscanf(fp, "%d", &voto);
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
            sleep(250);

            /* enviar señal SIGUSR1 a los demás procesos */
            for(i = 0; i < nprocs; i++) {
                kill(pids[i], SIGUSR1);
            }

        }

        if(got_SIGTERM) {
            break;
        }

    }

    free(pids);
    
    return EXIT_SUCCESS;
}