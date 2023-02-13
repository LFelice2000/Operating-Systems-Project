#ifndef _MINERO_H
#define _MINERO_H

typedef struct info_hilo{
    int upper;
    int lower;
} info_hilo;

/**
 * Funcion que inicializa la ronda de minado.
 * 
 * @param num_threads the number of threads that will be launched per miner.
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void round_init(int num_threads);

/**
 * Funcion que realiza el proof of work.
 * 
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void *prueba_de_fuerza(void *obj);

#endif