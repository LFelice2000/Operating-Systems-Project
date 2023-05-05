#ifndef MINER_FUNC_H
#define MINER_FUNC_H

#include "bloque.h"
#include "sistema.h"

void minado(int n_threads);

mqd_t queue_setup();

void check_voting(Sistema *sistema);

void count_votes(Sistema *sistema);

void vote(Sistema *sistema);

Sistema *firstminer_setup();

Sistema *miner_setup();

void voting_setup(Sistema *sistema, int target);

void send_signal(Sistema *sistema, int signal);

int send_to_monitor(Sistema *sistema, mqd_t mq);

int send_to_registrador(Sistema *sistema, int fd[2]);

#endif /* MINER_FUNC_H */
