#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pow.h"

pthread_t** create_threads(int num_threads,int obj);
void free_threads_memory(pthread_t** hilos, int num_hilos);

int main(int argc, char** argv) {
    long int test = 0;
    pthread_t** hilos = NULL;

    if(argc != 3) {
        fprintf(stderr, "Error: debes escribir ./minero <numero de rondas> <numero de hilos> <objetivo> para iniciar el programa.\n");
        return -1;
    }

    
    return 1;
}

pthread_t** create_threads(int num_threads, int obj) {
    pthread_t** hilos = NULL;
    int i, err;
    
    //Creo un array para guardar los hilos
    hilos = (pthread_t**)malloc(num_threads*sizeof(pthread_t*));

    for(i = 0; i < num_threads; i++) {
        
        hilos[i] = NULL;
        hilos[i] = (pthread_t*)malloc(sizeof(pthread_t));
        
        if(hilos[i] == NULL) {
            fprintf(stderr, "Error allocating thread memory\n");

            free_threads_memory(hilos, i);
        }

        err = pthread_create(hilos[i], NULL, pow_hash, obj);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            free_threads_memory(hilos, i);
            return -1;
        }

    }

    return hilos;

}

void free_threads_memory(pthread_t** hilos, int num_hilos) {
        int i;

        for(i = 0; i < num_hilos; i++) {
            free(hilos[i]);
            hilos[i] = NULL;
        }

        free(hilos);

        return; 
}