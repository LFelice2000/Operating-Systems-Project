#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pow.h"

pthread_t* create_threads(int num_threads,int obj);
void *prueba_de_fuerza(void *obj);

int main(int argc, char** argv) {
    long int test = 0;
    pthread_t** hilos = NULL;
    int rounds = 0, num_threads = 0, obj = 0;

    if(argc != 4) {
        fprintf(stderr, "Error: debes escribir ./minero <numero de rondas> <numero de hilos> <objetivo> para iniciar el programa.\n");
        return -1;
    }

    rounds = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    obj = atoi(argv[3]);
    
    create_threads(num_threads, obj);
    
    return 1;
}

pthread_t* create_threads(int num_threads, int obj) {
    pthread_t *hilos = NULL;
    int i, err, res;
    long int thread_result;
    
    /**Creo un array para guardar los hilos*/
    hilos = (pthread_t *)malloc(num_threads*sizeof(pthread_t));
    
    for(i = 0; i < num_threads; i++) {

        err = pthread_create(&hilos[i], NULL, prueba_de_fuerza, &obj);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            free(hilos);

            return NULL;
        }
    }


    for(i = 0; i < num_threads; i++) {

        err = pthread_join(hilos[i], (void **)&thread_result);
        if(err != 0) {
            fprintf(stderr, "pthread_join : %s\n", strerror(err));
            free(hilos);

            return NULL;
        }
    }

    printf("----------------------------------\nHilos: \n");
    for(i = 0; i < num_threads; i++) {

        printf("id: %ld\n", hilos[i]);
    }

    return NULL;

}

void *prueba_de_fuerza(void *obj) {
    int res = 0;
    int *pObj = obj;
    
    while (res != *pObj)
    {
        res = (rand() % (10000000 - 1)) + 1;
        if(res == *pObj) {
            printf("Hilo %ld encontro la solucion\n", pthread_self());
            fflush(stdout);

            return (void *)pthread_self();
        }
    }
    
    return NULL;
}