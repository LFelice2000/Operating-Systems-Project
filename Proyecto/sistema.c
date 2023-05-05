#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

#include "bloque.h"
#include "sistema.h"

#define MAX_MINEROS 100


void sistema_init(Sistema *sistema){

    int i;

    /* Inicialización del número de mineros a 0 */
    sistema->n_mineros = 0;

    /* Inicialización del número de bloques a 0 */
    sistema->n_bloques = 0;

    /* Inicialización de los pids a -1 */
    for(i = 0; i < MAX_MINEROS; i++){
        sistema->pids[i] = -1;
    }

    /* Inicialización de los votos a -1 */
    for(i = 0; i < MAX_MINEROS; i++){
        sistema->votes[i] = -1;
    }

    /* Inicialización de las carteras */
    for(i = 0; i < MAX_MINEROS; i++){
        sistema->wallets[i] = wallet_init();
    }

    /* Inicialización de los semáforos */
    if(sem_init(&(sistema->mutex), 1, 1) == -1){
        perror("[ERROR] No se ha podido inicializar el semáforo mutex\n");
        exit(EXIT_FAILURE);
    }

}

int sistema_add_minero(Sistema *sistema, pid_t pid){

    int i;

    /* Comprobar el número de mineros */
    if(sistema->n_mineros == MAX_MINEROS){
        return -1;
    }

    /* Atravesamos el array por si un minero en la posición 7 se va mientras que el último
        registrado está en la posición 15 */
    for(i = 0; i < MAX_MINEROS; i++){
        if(sistema->pids[i] == -1){
            sistema->pids[i] = pid;
            break;
        }
    }

    for(i = 0; i < MAX_MINEROS; i++){
        if(sistema->wallets[i].pid == -1){
            wallet_set_pid(&(sistema->wallets[i]), pid);
            wallet_set_coins(&(sistema->wallets[i]), 0);
            break;
        }
    }

    /* Aumentar el número de mineros */
    sistema->n_mineros++;

    return 0;

}

int sistema_remove_minero(Sistema *sistema, pid_t pid){

    int i;

    /* Atravesamos el array por si un minero en la posición 7 se va mientras que el último
        registrado está en la posición 15 */
    for(i = 0; i < MAX_MINEROS; i++){
        if(sistema->pids[i] == pid){
            sistema->pids[i] = -1;
            break;
        }
    }

    for(i = 0; i < MAX_MINEROS; i++){
        if(sistema->wallets[i].pid == pid){
            wallet_set_pid(&(sistema->wallets[i]), -1);
            wallet_set_coins(&(sistema->wallets[i]), 0);
            break;
        }
    }

    /* Disminuir el número de mineros */
    sistema->n_mineros--;

    return 0;

}

int sistema_get_nmineros(Sistema *sistema){

    int n_mineros;

    n_mineros = sistema->n_mineros;

    return n_mineros;

}

int sistema_get_nbloques(Sistema *sistema){

    int n_bloques;

    n_bloques = sistema->n_bloques;

    return n_bloques;

}

int sistema_inc_nbloques(Sistema *sistema){

    sistema->n_bloques++;

    return 0;

}

pid_t *sistema_get_pids(Sistema *sistema){

    pid_t *pids;

    pids = sistema->pids;

    return pids;

}

int *sistema_get_votes(Sistema *sistema){
    
    int *votes;

    votes = sistema->votes;

    return votes;
    
}

void sistema_set_bloque_votes(Sistema *sistema, int votes, int positives){


    bloque_set_votes(&sistema->current, votes);
    bloque_set_positives(&sistema->current, positives);
    
}

void sistema_reset_votes(Sistema *sistema){

    int i;

    for(i = 0; i < MAX_MINEROS; i++){
        sistema->votes[i] = -1;
    }

}

void sistema_set_current_bloque(Sistema *sistema, int target){


    sistema->current = bloque_init(sistema->n_bloques, target, sistema->wallets);
    sistema_inc_nbloques(sistema);
    
}

void sistema_set_last(Sistema *sistema){

    sistema->last = sistema->current;

}

Bloque sistema_get_current(Sistema *sistema){

    Bloque current;

    current = sistema->current;

    return current;

}

Bloque sistema_get_last(Sistema *sistema){

    Bloque last;

    last = sistema->last;

    return last;

}

void sistema_get_current_target(Sistema *sistema, int *target){

    *target = bloque_get_target(&(sistema->current));

}

void sistema_get_current_solution(Sistema *sistema, int *solution){

    *solution = bloque_get_solution(&(sistema->current));

}



void sistema_update_bloque(Sistema *sistema, int solution, pid_t pid){


    /* Actualizar el bloque */
    bloque_set_solution(&(sistema->current), solution);
    bloque_set_winner(&(sistema->current), pid);
    
}

void sistema_update_wallets(Sistema *sistema){

    int i;

    /* Actualizar las wallets */
    for(i = 0; i < MAX_MINEROS; i++){

        /* El minero ganador, gana una moneda y se guarda en la cartera */

        if(wallet_get_pid(&sistema->current.wallets[i]) == getpid()){
            wallet_inc_coins(&sistema->current.wallets[i]);
        }

        if(wallet_get_pid(&sistema->wallets[i]) == getpid()){
            wallet_inc_coins(&sistema->wallets[i]);
        }

    }


}

void sistema_destroy(Sistema *sistema){

    /* Destruir el semáforo */
    if(sem_destroy(&(sistema->mutex)) == -1){
        perror("[ERROR] No se ha podido destruir el semáforo mutex\n");
        exit(EXIT_FAILURE);
    }

}


