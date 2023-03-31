/* @file monitor.h
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-03-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MONITOR_H
#define MONITOR_H

#include <semaphore.h>

#define MAX_MSG 400

/**
 * @brief Estructura que representa un bloque
 * 
 */
typedef struct Bloque {
    char msg[MAX_MSG];
} Bloque;

/**
 * @brief Estructura a compartir entre procesos
 * 
 */
typedef struct shm_struct {
    Bloque buffer[6];
    sem_t sem_empty;
    sem_t sem_mutex;
    sem_t sem_fill;
    int front;
    int rear;
} shm_struct;

/**
 * @brief Función que implementa la funcionalidad del proceso Comprobador
 * 
 * @param lag milisegundos de retraso
 * @param fd_shm descriptor de fichero del segmento de memoria compartida
 */
void comprobador(int lag, int fd_shm);

/**
 * @brief Función que implementa la funcionalidad del proceso Monitor
 * 
 * @param lag milisegundos de retraso
 * @param fd_shm descriptor de fichero del segmento de memoria compartida
 */
void monitor(int lag, int fd_shm);

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

#endif // MONITOR_H