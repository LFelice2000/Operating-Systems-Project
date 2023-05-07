/**
 * @file sistema.h
 * @author Luis Felice y Angela Valderrama
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SISTEMA_H
#define SISTEMA_H

/**
 * @brief Estructura que representa el sistema de minado
 *
 */
typedef struct Sistema
{
    pid_t pids[MAX_MINEROS];
    int votes[MAX_MINEROS];
    Wallet wallets[MAX_MINEROS];
    int n_mineros;
    int n_bloques;
    Bloque last;
    Bloque current;
    sem_t mutex;
} Sistema;

/**
 * @brief Función que inicializa el sistema
 *
 * @param sistema objeto de tipo Sistema
 */
void sistema_init(Sistema *sistema);

/**
 * @brief Función que añade un minero al sistema
 *
 * @param sistema objeto de tipo Sistema
 * @param pid pid del minero a añadir
 * @return int 0 si se añade correctamente, -1 si no se añade
 */
int sistema_add_minero(Sistema *sistema, pid_t pid);

/**
 * @brief Función que elimina un minero del sistema
 *
 * @param sistema objeto de tipo Sistema
 * @param pid pid del minero a eliminar
 * @return int 0 si se elimina correctamente, -1 si no se elimina
 */
int sistema_remove_minero(Sistema *sistema, pid_t pid);

/**
 * @brief Función que devuelve el número de mineros en el sistema
 *
 * @param sistema objeto de tipo Sistema
 * @return int número de mineros en el sistema
 */
int sistema_get_nmineros(Sistema *sistema);

/**
 * @brief Función que devuelve el número de bloques en el sistema
 *
 * @param sistema objeto de tipo Sistema
 * @return int número de bloques en el sistema
 */
int sistema_get_nbloques(Sistema *sistema);

/**
 * @brief Función que incrementa el número de bloques en el sistema
 *
 * @param sistema objeto de tipo Sistema
 */
void sistema_inc_nbloques(Sistema *sistema);

/**
 * @brief Función que devuelve la lista de pids de los mineros en el sistema
 *
 * @param sistema objeto de tipo Sistema
 * @return pid_t* lista de pids de los mineros en el sistema
 */
pid_t *sistema_get_pids(Sistema *sistema);

/**
 * @brief Función que devuelve la lista de votos de los mineros en el sistema
 *
 * @param sistema objeto de tipo Sistema
 * @return int* lista de votos de los mineros en el sistema
 */
int *sistema_get_votes(Sistema *sistema);

/**
 * @brief Función que añade la información de la votación al bloque actual
 *
 * @param sistema objeto de tipo Sistema
 * @param votes número de votos
 * @param positives número de votos positivos
 */
void sistema_set_bloque_votes(Sistema *sistema, int votes, int positives);

/**
 * @brief Función que resetea los votos de los mineros
 *
 * @param sistema objeto de tipo Sistema
 */
void sistema_reset_votes(Sistema *sistema);

/**
 * @brief Función que establece la información del bloque actual
 *
 * @param sistema objeto de tipo Sistema
 * @param target valor del target a resolver
 */
void sistema_set_current_bloque(Sistema *sistema, int target);

/**
 * @brief Función que establece la información del bloque anterior
 *
 * @param sistema objeto de tipo Sistema
 */
void sistema_set_last(Sistema *sistema);

/**
 * @brief Función que devuelve el bloque actual
 *
 * @param sistema objeto de tipo Sistema
 * @return Bloque bloque actual
 */
Bloque sistema_get_current(Sistema *sistema);

/**
 * @brief Función que devuelve el bloque anterior
 *
 * @param sistema objeto de tipo Sistema
 * @return Bloque bloque anterior
 */
Bloque sistema_get_last(Sistema *sistema);

/**
 * @brief Función que devuelve el target del bloque actual
 *
 * @param sistema objeto de tipo Sistema
 * @param target variable donde se almacenará el target
 */
void sistema_get_current_target(Sistema *sistema, int *target);

/**
 * @brief Función que devuelve la solución del bloque actual
 *
 * @param sistema objeto de tipo Sistema
 * @param solution variable donde se almacenará la solución
 */
void sistema_get_current_solution(Sistema *sistema, int *solution);

/**
 * @brief Función que actualiza el bloque actual
 *
 * @param sistema objeto de tipo Sistema
 * @param solution solución del bloque actual
 * @param pid pid del minero que ha resuelto el bloque
 */
void sistema_update_bloque(Sistema *sistema, int solution, pid_t pid);

/**
 * @brief Función que actualiza las wallets de los mineros
 *
 * @param sistema objeto de tipo Sistema
 */
void sistema_update_wallets(Sistema *sistema);

/**
 * @brief Función que elimina el sistema y sus recursos
 *
 * @param sistema objeto de tipo Sistema
 */
void sistema_destroy(Sistema *sistema);

#endif /* SISTEMA_H */