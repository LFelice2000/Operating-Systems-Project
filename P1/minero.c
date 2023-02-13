#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "pow.h"
#include "minero.h"

int solucion = 0;
int target;

int main(int argc, char** argv) {
    int rounds = 0, num_threads = 0;
    int i = 0;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        return EXIT_FAILURE;
    }

    /* Parse the input values */
    target = atoi(argv[1]);
    rounds = atoi(argv[2]);
    num_threads = atoi(argv[3]);
    
    /* Lanzamos las rondas de minado */
    for(i = 0; i < rounds; i++){
        printf("Ronda %d empieza.\n------------------------\n", i+1);
        printf("Objetivo: %d\n", target);

        round_init(num_threads);

        /* Se guarda la solucion */
        solucion = 0;
        
        if(i != rounds-1){
            /* Se actualiza el objetivo */
            printf("Objetivo de la siguiente ronda: %d\n\n", target);
        }
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

    init = (int)floor((double)(POW_LIMIT)/num_threads)+1;
    end = init;
    
    /* Se crean y se lanzan los hilos que ejecutarán la prueba de fuerza */
    for(i = 0; i < num_threads; i++) {

        thInfo[i].lower = init * i;
        thInfo[i].upper = end - 1;

        end += init;
        if(end > POW_LIMIT) {
            end = POW_LIMIT;
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
 * @param info objective of the proof of power function.
 * 
 * @return 
*/
void *prueba_de_fuerza(void *info) {

    int res = 0, i = 0;
    info_hilo *thInfo = (info_hilo*)info;

    /* Se comprueba si ya se ha encontrado la solucion */
    i = thInfo->lower;
    while(solucion == 0){

        if(i > thInfo->upper) {
            break;
        }

        /* Se calcula el hash del numero aleatorio */
        res = pow_hash(i);
        /* Se comprueba si el hash es igual al objetivo */
        if(res == target) {
            printf("Hilo %ld encontró la solución %d para el objetivo %i\n", pthread_self(), i, res);
            /* Se guarda la solucion en una variable global para que
               los demás hilos sepan que tienen que parar de minar */
            solucion = 1;
            target = i;
            return NULL;
        }

        i++;
    }
    
    printf("Hilo %ld NO encontró solución, terminando...\n", pthread_self());
    
    return NULL;
}
