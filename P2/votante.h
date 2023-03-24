/**
 * @file votante.h
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-03-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef VOTANTE_H
#define VOTANTE_H

/**
 * @brief Funcion que se encarga de la competición entre los procesos votantes
 * 
 * @param nprocs número de procesos votantes
 * @param candsem semáforo para elegir candidato
 * @param votsem semáforo para acceder al archivo de votos
 */
void competicion(int nprocs, sem_t *candsem, sem_t *votsem);

/**
 * @brief Función que se encarga de la configuración de señales
 * 
 */
void señales();

/**
 * @brief Función que se encarga de la elección del candidato
 * 
 * @param nprocs número de procesos votantes
 * @param candsem semáforo para elegir candidato
 * @param votsem semáforo para acceder al archivo de votos
 */
void candidato(int nprocs, sem_t *candsem, sem_t *votsem);

/**
 * @brief Función que se encarga de la votación
 * 
 * @param votsem semáforo para acceder al archivo de votos
 */
void votante(sem_t *votsem);

#endif // VOTANTE_H