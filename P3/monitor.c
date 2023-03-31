/**
 * @file monitor.c
 * @author Luis Felice and Angela Valderrama
 * @brief 
 * @version 0.1
 * @date 2023-03-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <mqueue.h>
#include "monitor.h"

#define BUFFER_SIZE 6
#define SHM_NAME "/shm_monitor"
#define MQ_LEN 7
#define MQ_NAME "/queue_minero"

int main(int argc, char *argv[]) {

    int lag = 0;
    int fd_shm;
    
    if(argc < 2) {
        printf("Error, el programa debe iniciarse de la siguiente manera: ./monitor <LAG>");
        exit(EXIT_FAILURE);
    }

    /* Get argument */
    lag = atoi(argv[1]);

    /* Create shared memory segment or open it */
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd_shm == -1) {
        if (errno == EEXIST) {
            fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
            if (fd_shm == -1) {
                perror ("Error opening the shared memory segment\n");
                exit(EXIT_FAILURE);
            }
            printf("Soy monitor\n");
            monitor(lag, fd_shm);
            exit(EXIT_SUCCESS);
        } else {
            perror("Error creating the shared memory segment\n ") ;
            exit(EXIT_FAILURE) ;
        }
    }
    printf("Created shared memory segment: %d\n", fd_shm);
    comprobador(lag, fd_shm);

    return 0;
}

/**Productor*/
void comprobador(int lag, int fd_shm) {

    shm_struct *shm_struc = NULL;
    char recv[MAX_MSG] = "\0";
    int i = 0, exit_loop = 1, prior;
    mqd_t mq;
    struct mq_attr attributes;

    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = MAX_MSG;
    
    printf("[%d] checking blocks...\n", getpid());
    
    /* Inicializar segmento de memoria compartida */
    if (ftruncate(fd_shm , sizeof(shm_struct)) == -1) {
        perror ("ftruncate");
        shm_unlink ( SHM_NAME );
        exit (EXIT_FAILURE);
    }

    shm_struc = mmap(NULL, sizeof(shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0) ;
    close (fd_shm);
    if (shm_struc == MAP_FAILED) {
        perror ("mmap") ;
        shm_unlink(SHM_NAME);
        exit (EXIT_FAILURE);
    }

    init_struct(shm_struc);

    /* Recibe bloque de cola de mensajes */
    /* WHILE (1) */

    /* Introduce bloque en memoria compartida */
    i = 0;
    while(exit_loop){


        if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR, &attributes)) == ( mqd_t ) -1) {
            perror("mq_open") ;
            exit(EXIT_FAILURE) ;
        }
        
        fflush(stdout);
        printf("Wating for message...\n");

        if (mq_receive (mq, recv, MAX_MSG, &prior) == -1) {
            perror (" mq_receive ") ;
            mq_close (mq) ;
            exit ( EXIT_FAILURE ) ;
        }

        printf("Mensaje recibido: %s\n", recv);

        sem_wait(&shm_struc->sem_empty);
        sem_wait(&shm_struc->sem_mutex);

        shm_struc->rear++;
            if(shm_struc->rear == BUFFER_SIZE){
                shm_struc->rear = 0;
            }

        shm_struc->rear = shm_struc->rear % BUFFER_SIZE;
        
        if(strcmp(recv, "end") == 0) {
            printf("ending...");
            exit_loop = 0;
        } else {
            printf("Writing to shared memory segment...\n");
        }
        memcpy(shm_struc->buffer[shm_struc->rear].msg, recv, sizeof(recv));

        sem_post(&shm_struc->sem_mutex);
        sem_post(&shm_struc->sem_fill);
        
        sleep(lag);
        i++;
    }

    /* Destroy semaphores */
    sem_destroy(&shm_struc->sem_mutex);
    sem_destroy(&shm_struc->sem_empty);
    sem_destroy(&shm_struc->sem_fill);

    /* Unmap shared memory segment */
    munmap(shm_struc, sizeof(shm_struct));
    mq_close(mq) ;

    return;
}

void monitor(int lag, int fd_shm) {

    shm_struct *shm_struc = NULL;
    int end_loop = 1;
    printf("[%d] checking blocks...\n", getpid());
    
    /* Mapeo de segmento de memoria */
    shm_struc = mmap(NULL, sizeof(shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0) ;
    close(fd_shm);
    if (shm_struc == MAP_FAILED) {
        perror ("mmap") ;
        shm_unlink(SHM_NAME);
        exit (EXIT_FAILURE);
    }
    
    /* Extrae bloque de memoria compartida */
    while(end_loop){

        sem_wait(&shm_struc->sem_fill);
        sem_wait(&shm_struc->sem_mutex);

        shm_struc->front++;
        if(shm_struc->front == BUFFER_SIZE){
            shm_struc->front = 0;
        }

        shm_struc->front = shm_struc->front % BUFFER_SIZE;

        if(strcmp(shm_struc->buffer[shm_struc->front].msg, "end") == 0) {
            printf("ending...\n");
            end_loop = 0;
        }
        else {
            printf("The shared memory segment contains: %s\n", shm_struc->buffer[shm_struc->front].msg);
        }
        
        sem_post(&shm_struc->sem_mutex);
        sem_post(&shm_struc->sem_empty);

        sleep(lag);
    }

    /* Unmap shared memory segment */
    munmap(shm_struc, sizeof(shm_struct));
    shm_unlink (SHM_NAME);

    return;

}

void init_struct(shm_struct *shm_struc) {
    int i;

    if (shm_struc == NULL) {
        return;
    }

    shm_struc->front = -1;
    shm_struc->rear = -1;

    if(sem_init(&shm_struc->sem_mutex, 1, 1) == -1){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    if(sem_init(&shm_struc->sem_empty, 1, (BUFFER_SIZE - 1)) == -1){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&shm_struc->sem_fill, 1, 0) == -1){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }   
}