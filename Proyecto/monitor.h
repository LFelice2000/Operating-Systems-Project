/**
 * @file monitor.h
 * @author Luis Felice y Angela Valderrama
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <semaphore.h>
#include "miner.h"

/**
 * @brief Estructura a compartir entre Comprobador y Monitor
 *
 */
typedef struct shm_struct
{
    Bloque buffer[5];
    sem_t sem_empty;
    sem_t sem_mutex;
    sem_t sem_fill;
    int front;
    int rear;
} shm_struct;

/**
 * @brief Función que ejecuta la lógica del proceso Comprobador
 *
 */
void comprobador();

/**
 * @brief Función que ejecuta la lógica del proceso Monitor
 *
 */
void monitor();

/**
 * @brief Función que inicializa la estructura de memoria compartida
 *
 * @param shm_struc puntero a la estructura guardada en el segmento de memoria compartida
 */
void init_struct(shm_struct *shm_struc);

/**
 * @brief Función que comprueba si el resultado es valido para el objetivo
 *
 * @param target objetivo
 * @param res resultado
 * @return int 0 si no es valido, 1 si es valido
 */
int comprobar(int target, int res);

#endif /* MONITOR_H */