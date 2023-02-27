/**
 * @file minero.c
 * @author Luis Felice and Angela Valderrama.
 * @brief Implements the monitor process.
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
#include "minero.h"
#include "pow.h"

int monitor(int *request_validation, int *response_validation) {
    __ssize_t nbytes = 0;
    info_minero *info = NULL;
    int res = 0;

    /* Se crea la estructura de información del minero */
    info = (info_minero*)malloc(sizeof(info_minero));
    if(info == NULL){
        fprintf(stderr, "info_minero error\n");
        return EXIT_FAILURE;
    }


    do {

        /* Se cierra el pipe de escritura */
        close(request_validation[1]);

        /* Se lee del pipe de lectura */
        nbytes = read(request_validation[0], info, sizeof(info_minero));
        if (nbytes == -1){
            perror("read");
            return EXIT_FAILURE;
        }

        /* Se comprueba si el minero ha terminado */
        if(info->end == 1) {
            break;
        }
        
        /* Se comprueba si la solución es correcta */
        res = pow_hash(info->target);

        /* Se envía la respuesta al minero dependiendo */
        if(res == info->prevtarget) {
            printf("Solution accepted: %08d --> %08d\n", info->prevtarget, info->target);
            info->validation = 0;
            
        }else{
            printf("Solution rejected: %08d !-> %08d\n", info->prevtarget, info->target);
            info->validation = -1;
        }

        /* Se cierra el pipe de lectura */
        close(response_validation[0]);

        /* Se escribe en el pipe de escritura */
        nbytes = write(response_validation[1], info, sizeof(info_minero));
        if (nbytes == -1){
            perror("read");
            return EXIT_FAILURE;
        }

    } while(nbytes != 0);

    /* Se cierran los pipes */
    close(request_validation[1]);
    close(response_validation[0]);

    /* Se libera la memoria */
    free(info);
    
    return EXIT_SUCCESS;
    
}
