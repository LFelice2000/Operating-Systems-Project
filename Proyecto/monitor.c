/**
 * @file monitor.c
 * @author Luis Felice y Angela Valderrama
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <mqueue.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#include "bloque.h"
#include "miner_func.h"
#include "monitor.h"
#include "pow.h"

#define BUFFER_SIZE 5
#define MQ_LEN 10
#define MQ_NAME "/mq_monitor"
#define SHM_NAME "/shm_monitor"
#define SEM_MQUEUE "/sem_mqueue"

pid_t pid;
int fd_shm;
shm_struct *shm_struc;

int got_SIGINT = 0;

sigset_t set, oldset;
struct sigaction act;

sem_t *sem_mqueue;

struct mq_attr attributes;
mqd_t mq;

void handler(int sig)
{
    if (sig == SIGINT)
    {
        got_SIGINT = 1;
        clear();
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])
{

    /* El proceso Comprobador crea el segmento de memoria compartida */
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd_shm == -1)
    {
        if (errno == EEXIST)
        {
            fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
            if (fd_shm == -1)
            {
                perror("Error opening the shared memory segment\n");
                exit(EXIT_FAILURE);
            }
        }
    }

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

    /* Crear proceso Monitor */
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        monitor();
        exit(EXIT_SUCCESS);
    }
    else
    {
        comprobador();
        waitpid(pid, NULL, 0);
        if (got_SIGINT == 1)
        {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
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

void comprobador()
{

    int exit_loop = 0, prior, flag, value;
    Bloque bloque;

    sem_mqueue = sem_open(SEM_MQUEUE, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (sem_mqueue == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /* Preparar señal SIGINT */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Preparar cola de mensajes */
    attributes.mq_maxmsg = MQ_LEN;
    attributes.mq_msgsize = sizeof(Bloque);

    /* Abrir cola de mensajes */
    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    /* Introduce bloque en memoria compartida */
    while (exit_loop == 0)
    {

        /* Avisamos al minero de que puede enviar un bloque */
        sem_post(sem_mqueue);
        /* Recibe el bloque por la cola de mensajes */
        if (mq_receive(mq, (char *)&bloque, sizeof(Bloque), &prior) == -1)
        {
            perror("mq_receive");
            mq_close(mq);
            exit(EXIT_FAILURE);
        }

        /* Comprueba si es el bloque de finalización */
        if (bloque_get_target(&bloque) == -1)
        {
            exit_loop = 1;
        }
        else
        {
            /* Comprobar solución */
            flag = comprobar(bloque.target, bloque.solution);

            /* Preparar mensaje para enviar */
            bloque_set_flag(&bloque, flag);
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
        shm_struc->buffer[shm_struc->rear] = bloque;

        sem_post(&shm_struc->sem_mutex);
        sem_post(&shm_struc->sem_fill);
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

    /* Se cierra el semáforo */
    sem_close(sem_mqueue);
    sem_unlink(SEM_MQUEUE);
}

void monitor()
{

    Bloque bloque;
    Wallet *wallets;
    int end_loop = 0, i;

    /* Preparar señal SIGINT */
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) == -1)
    {
        perror("sigaction");
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
        bloque = shm_struc->buffer[shm_struc->front];
        if (bloque_get_target(&bloque) == -1)
        {
            end_loop = 1;
        }
        else
        {
            /* Leer objetivo y solución */

            printf("Id:       %04d\n", bloque_get_id(&bloque));
            printf("Winner:   %d\n", bloque_get_winner(&bloque));
            printf("Target:   %08d\n", bloque_get_target(&bloque));
            printf("Solution: %08d", bloque_get_solution(&bloque));
            if (bloque_get_flag(&bloque) == 1)
            {
                printf(" (validated)\n");
            }
            else
            {
                printf(" (rejected)\n");
            }

            printf("Votes:    %d/%d\n", bloque_get_positives(&bloque), bloque_get_votes(&bloque));
            printf("Wallets:  ");
            wallets = bloque_get_wallets(&bloque);
            for (i = 0; i < MAX_MINEROS; i++)
            {
                if (wallet_get_pid(wallets[i]) != -1)
                {
                    printf("%d:%02d ", wallet_get_pid(wallets[i]), wallet_get_coins(wallets[i]));
                }
            }
            printf("\n\n");
        }

        sem_post(&shm_struc->sem_mutex);
        sem_post(&shm_struc->sem_empty);
    }

    /* Se destruyen los semáforos sin nombre porque somos el último proceso en usarlos */
    sem_destroy(&shm_struc->sem_mutex);
    sem_destroy(&shm_struc->sem_empty);
    sem_destroy(&shm_struc->sem_fill);

    /* Se desasocia el segmento de memoria compartida */
    if (munmap(shm_struc, sizeof(shm_struct)) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    shm_unlink(SHM_NAME);

    return;
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

void clear()
{

    Bloque bloque;

    /* Bajamos el semáforo de la cola de mensajes para que no se pueda escribir en ella */
    sem_wait(sem_mqueue);

    bloque.target = -1;

    /* Se cierra la cola de mensajes */
    mq_close(mq);

    /* Se cierra el semáforo */
    sem_close(sem_mqueue);

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
    shm_struc->buffer[shm_struc->rear] = bloque;

    sem_post(&shm_struc->sem_mutex);
    sem_post(&shm_struc->sem_fill);

    /* Se desasocia el segmento de memoria compartida */
    if (munmap(shm_struc, sizeof(shm_struct)) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    waitpid(pid, NULL, 0);
}