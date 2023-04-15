/**
 * @file miner.c
 * @author Luis Felice and Angela Valderrama
 * @brief
 * @version 0.1
 * @date 2023-03-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mqueue.h>
#include "monitor.h"
#include "pow.h"

#define MQ_LEN 7
#define MQ_NAME "/queue_minero"

/**
 * @brief Función que realiza la prueba de fuerza
 *
 * @param target objetivo a resolver
 * @return int solución del problema de la prueba de fuerza
 */
int prueba_de_fuerza(int target);

int main(int argc, char *argv[])
{
    mqd_t mq;
    struct mq_attr attributes;
    int i, lag, nrounds = 0, target, res;
    char msg[MAX_MSG];

    if (argc < 3)
    {
        printf("Error, el programa debe ejecutarse como: ./monitor <ROUNDS> <LAG>\n");
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[1]) < 1)
    {
        printf("Error, el número de rondas debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[2]) < 0)
    {
        printf("Error, el lag debe ser mayor o igual que 0\n");
        exit(EXIT_FAILURE);
    }

    /* Obtener los argumentos */
    nrounds = atoi(argv[1]);
    lag = atoi(argv[2]);

    /* Crear la cola de mensajes */
    attributes.mq_maxmsg = MQ_LEN;
    attributes.mq_msgsize = MAX_MSG;

    if ((mq = mq_open(MQ_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    target = 0;
    printf("[%d] Generating blocks...\n", getpid());

    /* Empezar las rondas de prueba de fuerza */
    for (i = 0; i < nrounds; i++)
    {

        res = prueba_de_fuerza(target);

        /* El formato del mensaje será target-res */
        sprintf(msg, "%d-%d", target, res);

        /* Enviar el mensaje */
        if (mq_send(mq, msg, strlen(msg) + 1, 1) == -1)
        {
            perror("mq_send");
            mq_close(mq);
            exit(EXIT_FAILURE);
        }

        /* Esperar el tiempo de lag */
        usleep(lag);

        /* Obtener el nuevo target */
        target = res;
    }

    /* Enviar mensaje de finalización */
    strcpy(msg, "end");

    if (mq_send(mq, msg, strlen(msg) + 1, 1) == -1)
    {
        perror("mq_send");
        mq_close(mq);
        exit(EXIT_FAILURE);
    }

    /* Cerrar la cola de mensajes */
    mq_close(mq);

    printf("[%d] Finishing\n", getpid());

    exit(EXIT_SUCCESS);
}

int prueba_de_fuerza(int target)
{

    int i;

    for (i = 0; i < POW_LIMIT; i++)
    {
        if (pow_hash(i) == target)
        {
            break;
        }
    }

    return i;
}
