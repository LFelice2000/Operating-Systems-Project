#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pow.h"

void round_init(int num_threads,int obj);
void *prueba_de_fuerza(void *obj);

int solucion = -1;

int main(int argc, char** argv) {
    int rounds = 0, num_threads = 0, obj = 0;
    int i = 0;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <numero de rondas> <numero de hilos> <objetivo> para iniciar el programa.\n");
        return -1;
    }

    /**Parse the input values*/
    rounds = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    obj = atoi(argv[3]);
    
    /**Lanzamos las rondas de minado*/
    for(i = 0; i < rounds; i++){
        printf("Ronda %d empieza.\n------------------------\n", i+1);
        printf("Objetivo: %d\n", obj);

        round_init(num_threads, obj);

        /**Se guarda la solucion*/
        obj = solucion;
        solucion = -1;
       
        printf("Objetivo de la siguiente ronda: %d\n\n", obj);
    }

    printf("\nMinado terminado.\n");
    
    return 0;
}

/**
 * Funcion que inicializa la ronda de minado.
 * 
 * @param num_threads the number of threads that will be launched per miner.
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void round_init(int num_threads, int obj) {
    pthread_t *hilos = NULL;
    int i, err;
    
    /**Creo un array para guardar los hilos*/
    hilos = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
    
    /**Creo y lanzo los hilos*/
    for(i = 0; i < num_threads; i++) {

        err = pthread_create(&hilos[i], NULL, prueba_de_fuerza, &obj);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            
            free(hilos);

            return;
        }
    }

    /**Espero que terminen los hilos*/
    for(i = 0; i < num_threads; i++) {

        err = pthread_join(hilos[i], NULL);
        if(err != 0) {
            fprintf(stderr, "pthread_join : %s\n", strerror(err));
            
            free(hilos);

            return;
        }
    }

    free(hilos);

    return;
}

/**
 * Funcion que realiza el proof of work.
 * 
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void *prueba_de_fuerza(void *obj) {

    int res = 0, i = 0;
    int *pObj = (int *)obj;

    while(solucion == -1){
        i = (rand() % (1 - POW_LIMIT + 1)) + 1;
        res = pow_hash(i);
        if(res == *pObj) {
            printf("Hilo %ld encontro la solucion %d\n", pthread_self(), res);
            
            solucion = i;

            return NULL;
        }
    }
    
    printf("Hilo %ld NO encontro solucion, terminando...\n", pthread_self());
    
    return NULL;
}
