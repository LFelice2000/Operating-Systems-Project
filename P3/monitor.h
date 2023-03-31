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

typedef struct Bloque {
    char msg[MAX_MSG];
} Bloque;

typedef struct shm_struct {
    Bloque buffer[6];
    sem_t sem_empty;
    sem_t sem_mutex;
    sem_t sem_fill;
    int front;
    int rear;
} shm_struct;

void comprobador(int lag, int fd_shm);
void monitor(int lag, int fd_shm);
void init_struct(shm_struct *shm_struc);

#endif // MONITOR_H