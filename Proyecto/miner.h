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
    sem_t mutex;
    sem_t winner;
} Sistema;

typedef struct Info {
    int lower;
    int upper;
} Info;

typedef struct Minero {
    pthread_t *threads;
    Bloque *bloque;
} Minero;

void minero_main(int n_threads, int n_seconds);

void registrador();

void sistema_init(Sistema *sistema);

void bloque_init(Bloque *bloque, Sistema *sistema, int target);

Bloque *bloque_copy(Bloque *src);

void minado(int n_threads, Sistema *sistema);

void *prueba_de_fuerza(void *info);

void print_bloque(Bloque b);

void clear();

#endif /* MINER_H */