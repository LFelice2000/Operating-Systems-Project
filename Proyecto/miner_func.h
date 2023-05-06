#ifndef MINER_FUNC_H
#define MINER_FUNC_H

#include "bloque.h"
#include "sistema.h"

/**
 * @brief Funcion que ejecuta el minado de un bloque
 * 
 * @param n_threads numero de hilos a crear
 */
void minado(int n_threads);


/**
 * @brief Funcion que configura la cola de mensajes
 * 
 * @return mqd_t objeto de la cola de mensajes
 */
mqd_t queue_setup();


/**
 * @brief Funcion que comprueba si todos los mineros han votado
 * 
 * @param sistema objeto de tipo Sistema
 */
void check_voting(Sistema *sistema);


/**
 * @brief Funcion que cuenta los votos de los mineros
 * 
 * @param sistema objeto de tipo Sistema
 */
void count_votes(Sistema *sistema);


/**
 * @brief Función para establecer el voto de un minero
 * 
 * @param sistema objeto de tipo Sistema
 */
void vote(Sistema *sistema);


/**
 * @brief Funcion que crea el segmento de memoria compartida del sistema
 * 
 * @return Sistema* objeto de tipo Sistema 
 */
Sistema *firstminer_setup();


/**
 * @brief Funcion que abre el segmento de memoria compartida del sistema
 * 
 * @return Sistema* objeto de tipo Sistema
 */
Sistema *miner_setup();


/**
 * @brief Funcion que configura la votación del bloque ganador
 * 
 * @param sistema objeto de tipo Sistema
 * @param target solucion del bloque a minar
 */
void voting_setup(Sistema *sistema, int target);


/**
 * @brief Funcion que envia una señal a todos los mineros
 * 
 * @param sistema objeto de tipo Sistema
 * @param signal señal a enviar
 */
void send_signal(Sistema *sistema, int signal);


/**
 * @brief Funcion que envia un mensaje al Monitor mediante la cola de mensajes
 * 
 * @param sistema objeto de tipo Sistema
 * @param mq objeto de la cola de mensajes
 * @return int 0 si se ha enviado correctamente, -1 si no
 */
int send_to_monitor(Sistema *sistema, mqd_t mq);


/**
 * @brief Funcion que envia un mensaje al Registrador mediante el pipe
 * 
 * @param sistema objeto de tipo Sistema
 * @param fd descriptor de fichero del pipe
 * @return int 0 si se ha enviado correctamente, -1 si no
 */
int send_to_registrador(Sistema *sistema, int fd[2]);

#endif /* MINER_FUNC_H */
