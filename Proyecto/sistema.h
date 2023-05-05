#ifndef SISTEMA_H
#define SISTEMA_H

typedef struct Sistema{
    pid_t pids[MAX_MINEROS];
    int votes[MAX_MINEROS];
    Wallet wallets[MAX_MINEROS];
    int n_mineros;
    int n_bloques;
    Bloque last;
    Bloque current;
    sem_t mutex;
} Sistema;

void sistema_init(Sistema *sistema);

int sistema_add_minero(Sistema *sistema, pid_t pid);

int sistema_remove_minero(Sistema *sistema, pid_t pid);

int sistema_get_nmineros(Sistema *sistema);

int sistema_get_nbloques(Sistema *sistema);

int sistema_inc_nbloques(Sistema *sistema);

pid_t *sistema_get_pids(Sistema *sistema);

int *sistema_get_votes(Sistema *sistema);

void sistema_set_bloque_votes(Sistema *sistema, int votes, int positives);

void sistema_reset_votes(Sistema *sistema);

void sistema_set_current_bloque(Sistema *sistema, int target);

void sistema_set_last(Sistema *sistema);

Bloque sistema_get_current(Sistema *sistema);

Bloque sistema_get_last(Sistema *sistema);

void sistema_get_current_target(Sistema *sistema, int *target);

void sistema_get_current_solution(Sistema *sistema, int *solution);

void sistema_update_bloque(Sistema *sistema, int solution, pid_t pid);

void sistema_update_wallets(Sistema *sistema);

void sistema_destroy(Sistema *sistema);


#endif /* SISTEMA_H */