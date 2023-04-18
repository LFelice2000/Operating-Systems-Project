#ifndef MINER_H
#define MINER_H

#define MAX_MINEROS 100

typedef struct Wallet{
    int pid;
    int coins;
} Wallet;

typedef struct Bloque{
    int id;
    int target;
    int solution;
    pid_t winner;
    Wallet wallets[MAX_MINEROS];
    int votes;
    int positives;
} Bloque;

typedef struct Sistema{
    pid_t pids[MAX_MINEROS];
    int votes[MAX_MINEROS];
    int coins[MAX_MINEROS];
    int n_mineros;
    Bloque last;
    Bloque current; 
    sem_t winner;
    sem_t mutex;
} Sistema;

typedef struct Info {

    int lower;
    int upper;

} Info;

typedef struct Minero {
    pthread_t *threads;
} Minero;

void minero(int n_threads);

void registrador();

void sistema_init(Sistema *sistema);

void bloque_init(Bloque *bloque);

Minero *minero_init(int n_threads);

int minado(int n_threads, Sistema *sistema, Minero *minero);

void *prueba_de_fuerza(void *info);

#endif /* MINER_H */