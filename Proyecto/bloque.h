/**
 * @file bloque.h
 * @author Luis Felice y Angela Valderrama
 * @brief 
 * @version 0.1
 * @date 2023-05-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef BLOQUE_H
#define BLOQUE_H

#define MAX_MINEROS 100

/**
 * @brief Estructura que representa una wallet
 * 
 */
typedef struct Wallet{
    int pid;
    int coins;
} Wallet;


/**
 * @brief Estructura que representa un bloque
 * 
 */
typedef struct Bloque{
    int id;
    int target;
    int solution;
    pid_t winner;
    Wallet wallets[MAX_MINEROS];
    int votes;
    int positives;
    int flag;
} Bloque;


/**
 * @brief Función que inicializa una wallet
 * 
 * @return Wallet 
 */
Wallet wallet_init();


/**
 * @brief Función que establece el pid de una wallet
 * 
 * @param wallet objeto de tipo Wallet
 * @param pid pid del minero
 */
void wallet_set_pid(Wallet *wallet, int pid);


/**
 * @brief Función que devuelve el pid de una wallet
 * 
 * @param wallet objeto de tipo Wallet
 * @return pid_t pid del minero
 */
pid_t wallet_get_pid(Wallet wallet);


/**
 * @brief Función que devuelve el número de monedas de una wallet
 * 
 * @param wallet objeto de tipo Wallet
 * @return int número de monedas
 */
int wallet_get_coins(Wallet wallet);


/**
 * @brief Función que incrementa el número de monedas de una wallet
 * 
 * @param wallet objeto de tipo Wallet
 */
void wallet_inc_coins(Wallet *wallet);


/**
 * @brief Función que resetea el número de monedas de una wallet
 * 
 * @param wallet objeto de tipo Wallet
 * @param coins número de monedas
 */
void wallet_set_coins(Wallet *wallet, int coins);


/**
 * @brief Función que inicializa un bloque
 * 
 * @param id identificador del bloque
 * @param target objetivo del bloque
 * @param wallets wallets de los mineros
 * @return Bloque objeto de tipo Bloque inicializado
 */
Bloque bloque_init(int id, int target, Wallet *wallets);


/**
 * @brief Funcion que establece la solución de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @param solution solución del bloque
 * @return int 0 si se establece correctamente, -1 si no se establece
 */
int bloque_set_solution(Bloque *bloque, int solution);


/**
 * @brief 
 * 
 * @param bloque 
 * @param winner 
 * @return int 
 */
int bloque_set_winner(Bloque *bloque, pid_t winner);


/**
 * @brief Función que establece los votos de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @param votes número de votos del bloque
 * @return int 0 si se establece correctamente, -1 si no se establece
 */
int bloque_set_votes(Bloque *bloque, int votes);


/**
 * @brief Función que establece los votos positivos de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @param positives número de votos positivos del bloque
 * @return int 0 si se establece correctamente, -1 si no se establece
 */
int bloque_set_positives(Bloque *bloque, int positives);


/**
 * @brief Función que establece el flag que indica si el bloque 
 *      es válido o no por parte del Comprobador
 * 
 * @param bloque objeto de tipo Bloque
 * @param flag flag que indica si el bloque es válido o no
 * @return int 0 si se establece correctamente, -1 si no se establece
 */
int bloque_set_flag(Bloque *bloque, int flag);


/**
 * @brief Funcion que devuelve el identificador de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @return int 0 si se establece correctamente, -1 si no se establece
 */
int bloque_get_id(Bloque *bloque);


/**
 * @brief Función que devuelve el objetivo de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @return int target del bloque
 */
int bloque_get_target(Bloque *bloque);


/**
 * @brief Función que devuelve la solución de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @return int solución del bloque
 */
int bloque_get_solution(Bloque *bloque);


/**
 * @brief Función que devuelve el pid del minero ganador
 * 
 * @param bloque objeto de tipo Bloque
 * @return pid_t pid del minero ganador
 */
pid_t bloque_get_winner(Bloque *bloque);


/**
 * @brief Función que devuelve el número de votos de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @return int número de votos del bloque
 */
int bloque_get_votes(Bloque *bloque);


/**
 * @brief Función que devuelve el número de votos positivos de un bloque
 * 
 * @param bloque objeto de tipo Bloque
 * @return int número de votos positivos del bloque
 */
int bloque_get_positives(Bloque *bloque);


/**
 * @brief Función que devuelve el flag que indica si el bloque es válido o no
 * 
 * @param bloque objeto de tipo Bloque
 * @return int valor del flag que indica si el bloque es válido o no
 */
int bloque_get_flag(Bloque *bloque);


/**
 * @brief Función que devuelve las wallets de los mineros
 * 
 * @param bloque objeto de tipo Bloque
 * @return Wallet* wallets de los mineros
 */
Wallet *bloque_get_wallets(Bloque *bloque);


#endif /* BLOQUE_H */