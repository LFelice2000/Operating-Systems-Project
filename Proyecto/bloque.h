#ifndef BLOQUE_H
#define BLOQUE_H

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


Wallet wallet_init();

void wallet_set_pid(Wallet *wallet, int pid);

int wallet_get_pid(Wallet *wallet);

int wallet_get_coins(Wallet *wallet);

void wallet_inc_coins(Wallet *wallet);

void wallet_set_coins(Wallet *wallet, int coins);

Bloque bloque_init(int id, int target, Wallet *wallets);

void bloque_copy(Bloque *src, Bloque *dest);

int bloque_set_solution(Bloque *bloque, int solution);

int bloque_set_winner(Bloque *bloque, pid_t winner);

int bloque_set_votes(Bloque *bloque, int votes);

int bloque_set_positives(Bloque *bloque, int positives);

int bloque_set_flag(Bloque *bloque, int flag);

int bloque_get_id(Bloque *bloque);

int bloque_get_target(Bloque *bloque);

int bloque_get_solution(Bloque *bloque);

pid_t bloque_get_winner(Bloque *bloque);

int bloque_get_votes(Bloque *bloque);

int bloque_get_positives(Bloque *bloque);

int bloque_get_flag(Bloque *bloque);

int bloque_get_wallet_coins(Bloque *bloque, pid_t pid);

int bloque_increment_coins(Bloque *bloque, pid_t pid);


#endif /* BLOQUE_H */