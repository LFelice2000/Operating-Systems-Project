#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "pow.h"
#include "minero.h"

int solucion = 0;
int objetivo_comun = -1;

int main(int argc, char** argv) {
    int rounds = 0, num_threads = 0, obj = 0;
    int i = 0;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        return EXIT_FAILURE;
    }

    /* Parse the input values */
    obj = atoi(argv[1]);
    rounds = atoi(argv[2]);
    num_threads = atoi(argv[3]);
    
    /* Lanzamos las rondas de minado */
    for(i = 0; i < rounds; i++){
        printf("Ronda %d empieza.\n------------------------\n", i+1);
        printf("Objetivo: %d\n", obj);

        round_init(num_threads);

        /* Se guarda la solucion */
        solucion = 0;
       
        printf("Objetivo de la siguiente ronda: %d\n\n", obj);
    }

    printf("\nMinado terminado.\n");
    
    return EXIT_SUCCESS;
}

/**
 * Funcion que inicializa la ronda de minado.
 * 
 * @param num_threads the number of threads that will be launched per miner.
 * @param obj objective of the proof of power function.
 * 
 * @return 
*/
void round_init(int num_threads) {

    pthread_t *hilos = NULL;
    info_hilo *thInfo = NULL;
    int i, err, init, end;
    
    /* Se crea un array para guardar los hilos */
    hilos = (pthread_t *)malloc(num_threads*sizeof(pthread_t));

    thInfo = (info_hilo*)malloc(num_threads*sizeof(info_hilo));
    if(thInfo == NULL) {
        /**cosas*/
        return;
    }

    init = (int)ceil((double)(18 - 1)/num_threads);
    end = init;
    
    /* Se crean y se lanzan los hilos que ejecutarán la prueba de fuerza */
    for(i = 0; i < num_threads; i++) {

        thInfo[i].lower = i*init;
        thInfo[i].upper = end-1;
        end += end;
        if(end > 18) {
            end = 18;
        }

        printf("Empiezo %d, termino %d\n", thInfo[i].lower, thInfo[i].upper);

        err = pthread_create(&hilos[i], NULL, prueba_de_fuerza, (void *)&thInfo[i]);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            free(hilos);
            return;
        }
    
    }

    /* Se espera a que terminen los hilos*/
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
void *prueba_de_fuerza(void *info) {

    int res = 0, i = 0;
    info_hilo *thInfo = (info_hilo*)info;

    while(solucion == 0){

        srand(time(NULL));
        /* Se genera un numero aleatorio entre 0 y POW_LIMIT-1 */
        i = (rand() % (thInfo->upper - thInfo->lower + 1)) + thInfo->lower;

        /* Se calcula el hash del numero aleatorio */
        res = pow_hash(i);

        /* Se comprueba si el hash es igual al objetivo */
        if(res == objetivo_comun) {
            printf("Hilo %ld encontró la solución %d para el objetivo %i\n", pthread_self(), i, res);

            /* Se guarda la solucion en una variable global para que
               los demás hilos sepan que tienen que parar de minar */
            solucion = 1;
            objetivo_comun = i;
            return NULL;
        }
    }
    
    printf("Hilo %ld NO encontró solución, terminando...\n", pthread_self());
    
    return NULL;
}
