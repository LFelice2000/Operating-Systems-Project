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

    info = (info_minero*)malloc(sizeof(info_minero));
    if(info == NULL){
        fprintf(stderr, "info_minero error\n");
        return EXIT_FAILURE;
    }

    do {
        close(request_validation[1]);

        nbytes = read(request_validation[0], info, sizeof(info_minero));
        if (nbytes == -1){
            perror("read");
            return EXIT_FAILURE;
        }

        if(info->end == 1) {
            break;
        }
        
        res = pow_hash(info->target);

        if(res == info->prevtarget) {
            printf("Solution accepted: %d --> %d\n", info->prevtarget, info->target);
            info->validation = 0;
            close(response_validation[0]);
            nbytes = write(response_validation[1], info, sizeof(info_minero));
            if (nbytes == -1){
                perror("read");
                return EXIT_FAILURE;
            }
        }else{
            printf("Solution rejected: %d !-> %d\n", info->prevtarget, info->target);
            info->validation = -1;
            close(response_validation[0]);
            nbytes = write(response_validation[1], info, sizeof(info_minero));
            if (nbytes == -1){
                perror("read");
                return EXIT_FAILURE;
            }
        }
        

    } while(nbytes != 0);

    close(request_validation[1]);
    close(response_validation[0]);
    free(info);
    
    return EXIT_SUCCESS;
    
}
