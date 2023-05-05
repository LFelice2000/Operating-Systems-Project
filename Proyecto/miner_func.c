#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <semaphore.h>
#include <errno.h>


#include "miner_func.h"
#include "pow.h"

#define SHM_NAME "/shm_sistema"

#define MQ_LEN 10
#define MQ_NAME "/mq_monitor"

mqd_t queue_setup(){
    
    struct mq_attr attributes;
    mqd_t mq;

    /* Abrir la cola de mensajes */
    attributes.mq_maxmsg = MQ_LEN;
    attributes.mq_msgsize = sizeof(Bloque);

    if ((mq = mq_open(MQ_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    return mq;

}

void check_voting(Sistema *sistema){

    int i = 0, count = 0, j = 0;

    while(j < MAX_MINEROS){
        sem_wait(&sistema->mutex);
        for(i = 0; i < MAX_MINEROS; i++){
            if(sistema->votes[i] != -1){
                count++;
            }
        }
        
        if(count == sistema->n_mineros - 1){
            count = 0;
            sem_post(&sistema->mutex);
            break;
        }
        
        count = 0;
        sem_post(&sistema->mutex);
        
        usleep(500);
        j++;
    }

}

void count_votes(Sistema *sistema){

    int i = 0, yes = 0, total = 0, *votes;

    /* Se copian los votos en un array auxiliar */
    votes = sistema_get_votes(sistema);

    /* Se cuentan los votos */
    for(i = 0; i < MAX_MINEROS; i++){
        if(votes[i] != -1){
            total++;
            if(votes[i] == 1){
                yes++;
            }
        }
    }

    /* El propio ganador se vota a sí mismo */
    yes++;
    total++;

    /* Actualizar el número de votos del bloque ganador */
    sistema_set_bloque_votes(sistema, total, yes);

    if(yes >= total/2){

        /* Se actualizan las carteras */
        sistema_update_wallets(sistema);

    }

    /* Reset el contador de votos */
    sistema_reset_votes(sistema);

}

void vote(Sistema *sistema){

    int i = 0, target, solution, *votes;

    sistema_get_current_target(sistema, &target);
    sistema_get_current_solution(sistema, &solution);

    votes = sistema_get_votes(sistema);

    /* Se comprueba si la solución es válida */
    if(pow_hash(solution) == target){
        for(i = 0; i < MAX_MINEROS; i++){
            if(votes[i] == -1){
                votes[i] = 1;
                break;
            }
        }
    }
    else{
        for(i = 0; i < MAX_MINEROS; i++){
            if(votes[i] == -1){
                votes[i] = 0;
                break;
            }
        }
    }
    

}

Sistema *firstminer_setup(){

    Sistema *sistema;
    int fd_shm;

    /* Creación el segmento de memoria compartida */
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if(fd_shm == -1){
        perror("[ERROR] No se ha podido crear el segmento de memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    /* Establecer tamaño del segmento */
    if(ftruncate(fd_shm, sizeof(Sistema)) == -1){
        perror("[ERROR] No se ha podido establecer el tamaño del segmento de memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    /* Mapeo del segmento de memoria */
    sistema = mmap(NULL, sizeof(Sistema), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if(sistema == MAP_FAILED){
        perror("[ERROR] No se ha podido mapear el segmento de memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    return sistema;

}

Sistema *miner_setup(){

    Sistema *sistema;
    int fd_shm, statsem = 0;
    struct stat statshm;

    /* Abrir el segmento de memoria compartida */
    fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
    if(fd_shm == -1){
        perror("[ERROR] No se ha podido abrir el segmento de memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    /* Intento de mapeo checkeando el semáforo o la size...... (REVISAR) */

    /* Mapeo del segmento de memoria */
    sistema = mmap(NULL, sizeof(Sistema), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if(sistema == MAP_FAILED){
        perror("[ERROR] No se ha podido mapear el segmento de memoria compartida\n");
        exit(EXIT_FAILURE);
    }

    /* Comprobar el tamaño del segmento o el último semáforo */
    fstat(fd_shm, &statshm);
    sem_getvalue(&sistema->mutex, &statsem);
    while(statshm.st_size != sizeof(Sistema) || statsem != 0){
        fstat(fd_shm, &statshm);
        sem_getvalue(&sistema->mutex, &statsem);
    }

    return sistema;

}

void voting_setup(Sistema *sistema, int target){

    /* Actualización del bloque */
    sistema_update_bloque(sistema, target, getpid());

    /* Inicialización de los votos a -1 */
    sistema_reset_votes(sistema);

    /* Envío señal SIGUSR2 a los demás mineros */
    send_signal(sistema, SIGUSR2);

}

void send_signal(Sistema *sistema, int signal){

    int i = 0;
    pid_t *pids;

    pids = sistema_get_pids(sistema);

    /* Envío señal a los demás mineros */
    for(i = 0; i < MAX_MINEROS; i++){
        if(pids[i] != -1 && pids[i] != getpid()){
            kill(pids[i], signal);
        }
    }

}

int send_to_monitor(Sistema *sistema, mqd_t mq){

    Bloque bloque;

    bloque = sistema_get_current(sistema);

    if(mq_send(mq, (char *)&bloque, sizeof(Bloque), 0) == -1){
        if(errno == EAGAIN){
            printf("se cerró el monitor\n");
        }else{
            perror("mq_send normal");
            return -1;
        }
    }

    return 0;

}

int send_to_registrador(Sistema *sistema, int fd[2]){

    Bloque bloque;

    bloque = sistema_get_last(sistema);

    if(write(fd[1], &bloque, sizeof(Bloque)) == -1){
        perror("write");
        return -1;
    }

    return 0;

}

