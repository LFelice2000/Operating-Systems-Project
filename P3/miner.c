/**
 * @file miner.c
 * @author Luis Felice and Angela Valderrama
 * @brief 
 * @version 0.1
 * @date 2023-03-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "monitor.h"

#define MQ_LEN 7
#define MQ_NAME "/queue_minero"
#define TEST_MSG "test1", "test2", "test3", "test4", "test5", "test6", "test7", "end"

int main(int argc, char *argv[]) {
    mqd_t mq;
    struct mq_attr attributes;
    int i;
    char test[8][MAX_MSG] = {TEST_MSG};

    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = MAX_MSG;

    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == ( mqd_t ) -1) {
        perror("mq_open") ;
        exit(EXIT_FAILURE) ;
    }

    for(i = 0; i < 8; i++) {
        printf("Enviando mensaje: %s", test[i]);
        if (mq_send(mq, test[i], strlen(test[i]) + 1, 1) == -1) {
            perror("mq_send") ;
            mq_close(mq) ;
            exit(EXIT_FAILURE) ;
        }
    }

    mq_close(mq);

    return 0;
}
