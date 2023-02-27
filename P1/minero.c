/**
 * @file minero.c
 * @author Luis Felice and Angela Valderrama.
 * @brief 
 * @version 0.1
 * @date 2023-02-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include "pow.h"
#include "minero.h"
#include "monitor.h"

int solucion = 0;
int target;
int monitor_stat = 1;


int minero(int obj, int rounds, int num_threads) {

    int i = 0, status;
    pid_t pid;
    int request_validation[2], response_validation[2], pipe_stat, val_status = 0;
    ssize_t nbytes;
    info_minero *info = NULL;

    /* Se comprueba que los argumentos sean correctos */
    if(obj < 0 || rounds < 0 || num_threads < 0) {
        fprintf(stderr, "Error: debes escribir ./minero <TARGET> <ROUNDS> <THREADS> para iniciar el programa.\n");
        return EXIT_FAILURE;
    }

    /* Se guarda el target */
    target = obj;

    /* Se crean las tuberías */
    pipe_stat = pipe(request_validation);
    if(pipe_stat == -1){
        fprintf(stderr, "Pipe error\n");
        return EXIT_FAILURE;
    }

    pipe_stat = pipe(response_validation);
    if(pipe_stat == -1){
        fprintf(stderr, "Pipe error\n");
        return EXIT_FAILURE;
    }
    
    /* Se crea el proceso monitor */
    pid = fork();
    if(pid < 0) {
        printf("No se pudo lanzar el monitor.\n");
        return EXIT_FAILURE;
    } else if(pid == 0) {
        monitor_stat = monitor(request_validation, response_validation);
        exit(monitor_stat);
    }

    /* Se crea la estructura de información del minero */
    info = (info_minero*)calloc(1, sizeof(info_minero));
    if(info == NULL){
        fprintf(stderr, "info_minero error\n");
        return EXIT_FAILURE;
    }

    info->end = 0;
        
    /* Lanzamos las rondas de minado */
    for(i = 0; i < rounds; i++){
        
        /* Se guarda el target anterior */
        info->prevtarget = target;

        /* Empieza la ronda */
        round_init(num_threads);
        
        /* Se guarda la solucion */
        solucion = 0;

        /* Se calcula el nuevo target */
        info->target = target;

        /* Se cierra la tubería de lectura */
        close(request_validation[0]);

        /* Se escribe en la tubería de escritura */
        nbytes = write(request_validation[1], info, sizeof(info_minero));
        if(nbytes == -1){
            fprintf(stderr, "Write error\n");
            return EXIT_FAILURE;
        }

        /* Se cierra la tubería de escritura */
        close(response_validation[1]);

        /* Se lee de la tubería de lectura */
        nbytes = read(response_validation[0], info, sizeof(info_minero));
        if(nbytes == -1){
            fprintf(stderr, "Write error\n");
            return EXIT_FAILURE;
        }

        /* Se comprueba si la solución es válida */
        if(info->validation == -1) {
            val_status = -1;
            break;
        }
        
    }
    
    /* Se termina el minado */
    info->end = 1;

    /* Se cierra la tubería de lectura */
    close(request_validation[0]);

    /* Se escribe en la tubería de escritura */
    nbytes = write(request_validation[1], info, sizeof(info_minero));
    if(nbytes == -1){
        fprintf(stderr, "Write error\n");
        return EXIT_FAILURE;
    }
    
    /* Se comprueba si la solución es válida */
    if(val_status == -1) {
        printf("The solution has been invalidated\n");
    }

    /* Se espera a que termine el monitor */
    wait(&status);

    /* Se comprueba el estado de salida del monitor */
    if(WIFEXITED(status)) {
        monitor_stat = WEXITSTATUS(status);
        printf("Monitor exited with status %d\n", monitor_stat);
    } else {
        printf("Monitor exited unexpectedly\n");
    }

    /* Se cierran las tuberías */
    close(request_validation[0]);
    close(response_validation[1]);

    /* Se libera la memoria */
    free(info);

    /* Si la respuesta era inválida, se devuelve EXIT_FAILURE */
    if(val_status == -1 || status == 1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void round_init(int num_threads) {

    pthread_t *hilos = NULL;
    info_hilo *thInfo = NULL;
    int i, err, init, end;
    
    /* Se crea un array para guardar los hilos */
    hilos = (pthread_t *)malloc(num_threads*sizeof(pthread_t));

    /* Se crea un array para guardar la información de los hilos */
    thInfo = (info_hilo*)malloc(num_threads*sizeof(info_hilo));
    if(thInfo == NULL) {
        /**cosas*/
        return;
    }

    /* Se calcula el número de iteraciones que realizará cada hilo */
    init = (int)floor((double)(POW_LIMIT)/num_threads)+1;
    end = init;
    
    /* Se crean y se lanzan los hilos que ejecutarán la prueba de fuerza */
    for(i = 0; i < num_threads; i++) {
        
        /* Se guarda la información de cada hilo */
        thInfo[i].lower = init * i;
        thInfo[i].upper = end - 1;

        /* Se comprueba que el rango de iteraciones no se salga del límite */
        end += init;
        if(end > POW_LIMIT) {
            end = POW_LIMIT;
        }

        /* Se crea el hilo */
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

    /* Se libera la memoria */
    free(hilos);
    free(thInfo);

    return;
}

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
            
            /* Se guarda la solucion en una variable global para que
               los demás hilos sepan que tienen que parar de minar */
            solucion = 1;
            target = i;

            return NULL;
        }

        i++;
    }
    
    return NULL;
}