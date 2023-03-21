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

int got_SIGUSR1 = 0;
int got_SIGUSR2 = 0;

pid_t *pids = NULL;

void sighandler(int sig) {
    if(sig == SIGUSR1){
        got_SIGUSR1 = 1;
    }else if(sig == SIGUSR2){
        got_SIGUSR2 = 1;
    }else if(sig == SIGTERM){
        free(pids);
        exit(EXIT_SUCCESS);
    }
}

int votante(int nprocs) {

    FILE *fp = NULL;
    pid_t pid;
    char bufpid[30] = "\0";
    int voto = 0, i = 0, yes = 0, no = 0;
    sigset_t set, oldset;
    struct sigaction act;
    sem_t *candsem = NULL, *votsem = NULL, *sigsem = NULL;

    pids = (pid_t *) malloc((nprocs-1) * sizeof(pid_t));
    if(pids == NULL) {
        perror("malloc");
        exit(1);
    }

    /* configurar la captura de señales */
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    /* bloquear señales */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    sigprocmask(SIG_BLOCK, &set, &oldset);

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

    /* abrir semáforo de candidato */
    candsem = sem_open("candsem", 0);
    if(candsem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* abrir semáforo de votos */
    votsem = sem_open("votsem", 0);
    if(votsem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* esperar a que el sistema esté listo y reciba SIGUSR1 */
    sigsuspend(&oldset);

    if(got_SIGUSR1) {
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
    }

    while(got_SIGUSR1) {

        got_SIGUSR1 = 0;

        /* decidir si soy candidato o no */
        if(sem_trywait(candsem) == 0) {

            /* soy candidato */
            
            /* abrir fichero para comprobar los votos */
            fp = fopen("votos.txt", "w");
            if(fp == NULL) {
                perror("fopen");
                return EXIT_FAILURE;
            }
            fclose(fp);

            /* enviar señal SIGUSR2 a los demás procesos */
            for(i = 0; i < nprocs-1; i++) {
                kill(pids[i], SIGUSR2);
            }

            while(1){

                sem_wait(votsem);

                /* abrir fichero para leer los votos */
                fp = fopen("votos.txt", "r");
                if(fp == NULL) {
                    perror("fopen");
                    return EXIT_FAILURE;
                }

                /* comprobar si todos los votos han sido contabilizados */
                fseek(fp, 0, SEEK_END);
                if(ftell(fp) == ((nprocs-1)*2)){
                    rewind(fp);
                    fclose(fp);

                    /* todos los votos han sido contabilizados */
                    fprintf(stdout, "Candidate %d => [ ", getpid());

                    fp = fopen("votos.txt", "r");

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

                    /* enviar señal SIGUSR1 a los demás procesos */
                    for(i = 0; i < nprocs - 1; i++) {
                        kill(pids[i], SIGUSR1);
                    }

                    usleep(250000);

                    got_SIGUSR1 = 1;

                    break;
                }
                rewind(fp);
                fclose(fp);

                sem_post(votsem);

                usleep(10000);

            }

        }else{

            /* no soy candidato */
            sigsuspend(&oldset);

            /* zona crítica de escritura */
            sem_wait(votsem);

            /* abrir fichero para escribir los votos */
            fp = fopen("votos.txt", "a+");
            if(fp == NULL) {
                perror("fopen");
                return EXIT_FAILURE;
            }

            /* decidir voto aleatoriamente entre 0 y 1 */
            srand(time(NULL));
            voto = rand() % 2;

            /* escribir voto en el fichero */
            fprintf(fp, "%d\n", voto);
            fclose(fp);
            sem_post(votsem);
            
            /* termina la zona crítica */
            got_SIGUSR2 = 0;

            /* esperar a recibir SIGUSR1 */
            sigsuspend(&oldset);

        }

    }

    free(pids);
    exit(EXIT_SUCCESS);

}