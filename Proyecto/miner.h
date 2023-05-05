#ifndef MINER_H
#define MINER_H

#define MAX_MINEROS 100

typedef struct Info {
    int lower;
    int upper;
} Info;

void minero(int n_threads, int n_seconds);

void registrador();

void minado(int n_threads);

void *prueba_de_fuerza(void *info);

void signals(int n_seconds);

void clear();

#endif /* MINER_H */