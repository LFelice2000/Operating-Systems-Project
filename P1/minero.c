#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pow.h"

int round_init(int num_threads,int obj);
void *prueba_de_fuerza(void *obj);

int main(int argc, char** argv) {
    long int test = 0;
    pthread_t** hilos = NULL;
    int rounds = 0, num_threads = 0, obj = 0;
    int i = 0;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <numero de rondas> <numero de hilos> <objetivo> para iniciar el programa.\n");
        return -1;
    }

    rounds = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    obj = atoi(argv[3]);
    
    for(i = 0; i < rounds; i++){
        printf("Ronda %d empieza.\n", i+1);
        printf("Objetivo: %d\n", obj);

        obj = round_init(num_threads, obj);
        printf("Objetivo de la siguiente ronda: %d\n", obj);
        if(obj == -1) {
            fprintf(stderr, "Error: no se pudo empezar la ronda.\n");
            return -1;
        }
    }
    
    return 0;
}

int round_init(int num_threads, int obj) {
    pthread_t *hilos = NULL;
    int i, err, res;
    int thread_result;
    
    /**Creo un array para guardar los hilos*/
    hilos = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
    
    /**Creo y lanzo los hilos*/
    for(i = 0; i < num_threads; i++) {

        err = pthread_create(&hilos[i], NULL, prueba_de_fuerza, &obj);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            free(hilos);

            return -1;
        }
    }

    /**Espero que terminen los hilos*/
    for(i = 0; i < num_threads; i++) {

        err = pthread_join(hilos[i], (void **)&thread_result);
        if(err != 0) {
            fprintf(stderr, "pthread_join : %s\n", strerror(err));
            free(hilos);

            return -1;
        }
    }

    printf("----------------------------------\nHilos: \n");
    for(i = 0; i < num_threads; i++) {

        printf("id: %ld\n", hilos[i]);
    }

    return thread_result;
}

void *prueba_de_fuerza(void *obj) {

    int res = 0, i = 0;
    int *pObj = (int *)obj;

    while(res != *pObj){
        i = rand() % POW_LIMIT-1;
        res = pow_hash(i);
        if(res == *pObj) {
            printf("Hilo %ld encontro la solucion para %d: %d\n", pthread_self(), res, i);
            exit(i);
        }
    }
    
    return NULL;
}