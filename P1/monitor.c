#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minero.h"

int monitor(int *fd) {
    char buf[10000], *strings;
    __ssize_t nbytes = 0;
    info_minero *info = NULL;
    
    info = (info_minero*)malloc(sizeof(info_minero));
    if(info == NULL){
        fprintf(stderr, "info_minero error\n");
        return EXIT_FAILURE;
    }

    do {
        close(fd[1]);

        nbytes = read(fd[0], info, sizeof(info));
        if (nbytes == -1){
            fprintf(stderr, "Read error\n");
            return EXIT_FAILURE;
        }

        if(info->end == 1) {
            printf("[MONITOR]: Me llego el exit\n");
            break;
        }
        else {
            printf("[MONITOR]: comprobando solucion %d\n", info->target);
        }

    } while(nbytes != 0);
    
    return EXIT_SUCCESS;
    
}
