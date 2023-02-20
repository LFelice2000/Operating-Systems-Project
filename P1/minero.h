#ifndef _MINERO_H
#define _MINERO_H

typedef struct info_minero
{
    int prevtarget;
    int target;
    int end;

} info_minero;

typedef struct info_hilo
{
    int upper;
    int lower;

} info_hilo;

/**
 * @brief Funcion que realiza el minado de la cadena.
 * 
 * @param target target of the proof of power function.
 * @param round number of rounds to mine.
 * @param num_threads number of threads that will be launched per miner.
 * @return int EXIT_SUCCESS if the miner has mined the chain, EXIT_FAILURE otherwise.
 */
int minero(int target, int round, int num_threads);

/**
 * Function that initializes the mining round.
 * 
 * @param num_threads the number of threads that will be launched per miner.
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void round_init(int num_threads);

/**
 * Function that performs the proof of power function.
 * 
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void *prueba_de_fuerza(void *obj);

#endif