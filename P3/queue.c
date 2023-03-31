#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "queue.h"

mqd_t mq_init(int size, int msg_len) {
    struct mq_attr attributes;
    mqd_t mq;

    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = MAX_MSG;

    if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == ( mqd_t ) -1) {
        perror("mq_open") ;
        exit(EXIT_FAILURE) ;
    }
    
}