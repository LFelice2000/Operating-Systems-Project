/**
 * @file miner.h
 * @author Luis Felice y Angela Valderrama
 * @brief
 * @version 0.1
 * @date 2023-05-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MINER_H
#define MINER_H

#define MAX_MINEROS 100

/**
 * @brief Estructura que representa la información de un hilo
 *
 */
typedef struct Info
{
    int lower;
    int upper;
} Info;

/**
 * @brief Funcion que ejecuta la funcionalidad del minero
 *
 * @param n_threads numero de hilos a crear
 * @param n_seconds número de segundos a trabajar
 */
void minero(int n_threads, int n_seconds);

/**
 * @brief Funcion que ejecuta la funcionalidad del registrador
 *
 */
void registrador();

/**
 * @brief Funcion que ejecuta el minado de un bloque
 *
 * @param n_threads numero de hilos a crear
 */
void minado(int n_threads);

/**
 * @brief Funcion que ejecuta un hilo de minado
 *
 * @param info objeto de tipo Info
 * @return void* objeto a devolver
 */
void *prueba_de_fuerza(void *info);

/**
 * @brief Funcion que configura las señales
 *
 * @param n_seconds numero de segundos a trabajar
 */
void signals(int n_seconds);

/**
 * @brief Funcion que ejecuta la limpieza de recursos
 *
 */
void clear();

#endif /* MINER_H */