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
    int flag;
} Bloque;

typedef struct Sistema{
    pid_t pids[MAX_MINEROS];
    int votes[MAX_MINEROS];
    Wallet wallets[MAX_MINEROS];
    int n_mineros;
    int n_bloques;
    Bloque last;
    Bloque current;
    Bloque bwinner;
    sem_t mutex;
    sem_t winner;
    sem_t ronda;
} Sistema;

typedef struct Info {
    int lower;
    int upper;
} Info;

typedef struct Minero {
    pthread_t *threads;
    Info *thInfo;
} Minero;

void minero_main(int n_threads, int n_seconds);

void registrador();

void sistema_init(Sistema *sistema);

void bloque_init(Bloque *bloque, Sistema *sistema, int given_target);

void bloque_copy(Bloque *src, Bloque *dest);

void minado(int n_threads, Sistema *sistema);

void *prueba_de_fuerza(void *info);

void print_bloque(Bloque b);

void signals(int n_seconds);

void clear();

#endif /* MINER_H */