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
#include <semaphore.h>
#include "monitor.h"
#include "pow.h"

#define BUFFER_SIZE 6
#define SHM_NAME "/shm_monitor"
#define MQ_LEN 7
#define MQ_NAME "/queue_minero"

int main(int argc, char *argv[])
{

    int lag = 0;
    int fd_shm;

    if (argc < 2)
    {
        printf("Error, el programa debe iniciarse de la siguiente manera: ./monitor <LAG>\n");
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[1]) < 0)
    {
        printf("Error, el lag debe ser mayor o igual que 0\n");
        exit(EXIT_FAILURE);
    }

    /* Obtener los argumentos */
    lag = atoi(argv[1]);

    /* Crear segmento de memoria compartida o abrirlo si ya existe */
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd_shm == -1)
    {
        if (errno == EEXIST)
        {
            /* Si ya existe, soy el proceso Monitor y abro el segmento de memoria */
            fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
            if (fd_shm == -1)
            {
                perror("Error opening the shared memory segment\n");
                exit(EXIT_FAILURE);
            }

            monitor(lag, fd_shm);
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror("Error creating the shared memory segment\n ");
            exit(EXIT_FAILURE);
        }
    }

    /* He creado el segmento de memoria y soy el proceso Comprobador */
    comprobador(lag, fd_shm);

    return 0;
}

void comprobador(int lag, int fd_shm)
{

    shm_struct *shm_struc = NULL;
    struct mq_attr attributes;
    mqd_t mq;
    char recv[MAX_MSG] = "\0";
    int exit_loop = 0, prior, flag;
    long int target, res;

    attributes.mq_maxmsg = MQ_LEN;
    attributes.mq_msgsize = MAX_MSG;

    printf("[%d] Checking blocks...\n", getpid());

    /* Establecer tamaño del segmento de memoria compartida */
    if (ftruncate(fd_shm, sizeof(shm_struct)) == -1)
    {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    /* Mapeo de segmento de memoria */
    shm_struc = mmap(NULL, sizeof(shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (shm_struc == MAP_FAILED)
    {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    /* Inicializar estructura de memoria compartida */
    init_struct(shm_struc);

    /* Abrir cola de mensajes */
    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDONLY , S_IRUSR | S_IWUSR, &attributes)) == (mqd_t) -1) {
        perror("mq_open") ;
        exit(EXIT_FAILURE) ;
    }

    /* Introduce bloque en memoria compartida */
    while(exit_loop == 0){

        fflush(stdout);

        /* Recibe el bloque por la cola de mensajes */
        if (mq_receive(mq, recv, MAX_MSG, &prior) == -1)
        {
            perror("mq_receive");
            mq_close(mq);
            exit(EXIT_FAILURE);
        }

        /* Comprueba si es el bloque de finalización */
        if (strcmp(recv, "end") == 0)
        {
            exit_loop = 1;
        }
        else
        {
            /* Obtener el target y la solución */
            sscanf(recv, "%ld-%ld", &target, &res);

            /* Comprobar solución */
            flag = comprobar(target, res);

            /* Preparar mensaje para enviar */
            sprintf(recv, "%ld %ld %d", target, res, flag);

        }

        /* Espera a que haya espacio en el buffer */
        sem_wait(&shm_struc->sem_empty);

        /* Protegemos el acceso a memoria compartida */
        sem_wait(&shm_struc->sem_mutex);

        /* Escribir bloque en memoria compartida */
        shm_struc->rear++;
        if (shm_struc->rear == BUFFER_SIZE)
        {
            shm_struc->rear = 0;
        }

        shm_struc->rear = shm_struc->rear % BUFFER_SIZE;
        memcpy(shm_struc->buffer[shm_struc->rear].msg, recv, sizeof(recv));

        sem_post(&shm_struc->sem_mutex);
        sem_post(&shm_struc->sem_fill);

        /* Espera */
        usleep(lag*1000);
    }

    /* Se desasocia el segmento de memoria compartida */
    if (munmap(shm_struc, sizeof(shm_struct)) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    /* Se cierra la cola de mensajes y se elimina porque somos el último proceso en usarla */
    mq_close(mq);
    mq_unlink(MQ_NAME);

    printf("[%d] Finishing\n", getpid());

    return;
}

void monitor(int lag, int fd_shm)
{

    shm_struct *shm_struc = NULL;
    int end_loop = 0, flag;
    long int target, res;

    printf("[%d] Printing blocks...\n", getpid());

    /* Mapeo de segmento de memoria */
    shm_struc = mmap(NULL, sizeof(shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if (shm_struc == MAP_FAILED)
    {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    /* Extrae bloque de memoria compartida */
    while (end_loop == 0)
    {

        /* Espera a que haya un bloque en el buffer */
        sem_wait(&shm_struc->sem_fill);
        /* Protegemos el acceso a memoria compartida */
        sem_wait(&shm_struc->sem_mutex);

        /* Leer bloque de memoria compartida */
        shm_struc->front++;
        if (shm_struc->front == BUFFER_SIZE)
        {
            shm_struc->front = 0;
        }

        shm_struc->front = shm_struc->front % BUFFER_SIZE;

        /* Comprueba si es el bloque de finalización */
        if (strcmp(shm_struc->buffer[shm_struc->front].msg, "end") == 0)
        {
            end_loop = 1;
        }
        else
        {
            /* Leer objetivo y solución */
            sscanf(shm_struc->buffer[shm_struc->front].msg, "%ld %ld %d", &target, &res, &flag);

            if (flag == 1)
            {
                printf("Solution accepted: %08ld --> %08ld\n", target, res);
            }
            else
            {
                printf("Solution rejected: %08ld --> %08ld\n", target, res);
            }
        }

        sem_post(&shm_struc->sem_mutex);
        sem_post(&shm_struc->sem_empty);

        /* Espera */
        usleep(lag*1000);
    }

    /* Se destruyen los semáforos sin nombre porque somos el último proceso en usarlos */
    sem_destroy(&shm_struc->sem_mutex);
    sem_destroy(&shm_struc->sem_empty);
    sem_destroy(&shm_struc->sem_fill);

    /* Se desasocia el segmento de memoria compartida */
    if(munmap(shm_struc, sizeof(shm_struct)) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    shm_unlink(SHM_NAME);

    printf("[%d] Finishing\n", getpid());

    return;
}

void init_struct(shm_struct *shm_struc)
{

    if (shm_struc == NULL)
    {
        return;
    }

    /* Inicializar front y rear del buffer circular */
    shm_struc->front = -1;
    shm_struc->rear = -1;

    /* Inicializar semáforos sin nombre a utilizar */
    if (sem_init(&shm_struc->sem_mutex, 1, 1) == -1)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shm_struc->sem_empty, 1, (BUFFER_SIZE - 1)) == -1)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shm_struc->sem_fill, 1, 0) == -1)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

int comprobar(int target, int res)
{

    int flag = 0;

    if (target == pow_hash(res))
    {
        flag = 1;
    }

    return flag;
}