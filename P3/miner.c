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
#include <unistd.h>
#include <mqueue.h>
#include "monitor.h"

#define MQ_LEN 7
#define MQ_NAME "/queue_minero"
#define TEST_MSG "test1", "test2", "test3", "test4", "test5", "test6", "test7", "end"

int main(int argc, char *argv[]) {
    mqd_t mq;
    struct mq_attr attributes;
    int i, lag;
    char test[8][MAX_MSG] = {TEST_MSG};

    if(argc != 2) {
        printf("Error, el programa debe ejecutarse como: ./monitor <LAG>\n");
        exit(EXIT_FAILURE);
    }

    lag = atoi(argv[1]);

    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = MAX_MSG;

    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == ( mqd_t ) -1) {
        perror("mq_open") ;
        exit(EXIT_FAILURE) ;
    }

    for(i = 0; i < 8; i++) {
        printf("Enviando mensaje: %s\n", test[i]);
        if (mq_send(mq, test[i], strlen(test[i]) + 1, 1) == -1) {
            perror("mq_send") ;
            mq_close(mq) ;
            exit(EXIT_FAILURE) ;
        }

        sleep(lag);
    }

    mq_close(mq);

    exit(EXIT_SUCCESS);
}
