#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>

int got_SIGUSR1 = 0;
int got_SIGUSR2 = 0;

void usr1handler(int sig) {
    got_SIGUSR1 = 1;
}

void usr2handler(int sig) {
    got_SIGUSR2 = 1;
}

int votante(int nprocs, int candsem) {

    FILE *fp = NULL;
    struct sigaction act;
    pid_t *pids = NULL, pid;
    int voto = 0, i = 0;

    pids = (pid_t *) malloc((nprocs) * sizeof(pid_t));
    if(pids == NULL) {
        perror("malloc");
        exit(1);
    }

    /* Se configura la captura de señales */
    act.sa_handler = usr1handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGUSR1, &act, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    act.sa_handler = usr2handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGUSR2, &act, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    /* Cuando el sistema esté listo */
    while(got_SIGUSR1) {

        /* abrir fichero para leer los pids */
        fp = fopen("pid.txt", "r");
        if(fp == NULL) {
            perror("fopen");
            exit(1);
        }

        /* leer información del fichero para saber los pids */
        i = 0;
        while(!feof(fp) && i < nprocs) {
            fscanf(fp, "%d", pid);
            /* si el pid es distinto del mio, lo guardo */
            if(pid != getpid()) {
                pids[i] = pid;
                i++;
            }
        }

        fclose(fp);

        /* decidir si soy candidato o no */
        /* semáforo !!! */

        sem_trywait(&sem);

        if(candidato){
            /* abrir fichero para comprobar los votos */
            fp = fopen("votos.txt", "w+");
            if(fp == NULL) {
                perror("fopen");
                return EXIT_FAILURE;
            }

            /* enviar señal SIGUSR2 a los demás procesos */
            for(i = 0; i < nprocs; i++) {
                kill(pids[i], SIGUSR2);
            }

            /* comprobar que todos han votado */
            while(1) {
                fseek(fp, 0, SEEK_END);
                if(ftell(fp) == nprocs) {
                    frewind(fp);
                    break;
                }
                frewind(fp);
                sleep(1000);
            }

            fprintf(stdout, "Candidate %d => [", getpid());

            /* leer votos */
            while(!feof(fp)) {
                fscanf(fp, "%d", voto);
                if(voto == 0) {
                    fprintf(stdout, "N");
                    no++;
                } else {
                    fprintf(stdout, "Y");
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

        /* cuando reciba la señal SIGUSR2 */
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

            /* esperar a que termine la votación */
            while(1){
                sleep(1);
            }
            
        }





    }


    
    return 0;
}