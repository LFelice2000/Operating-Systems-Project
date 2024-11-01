/**
 * @file bloque.c
 * @author Luis Felice y Angela Valderrama
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "bloque.h"

Wallet wallet_init()
{

    Wallet wallet;

    wallet.pid = -1;
    wallet.coins = 0;

    return wallet;
}

void wallet_set_pid(Wallet *wallet, int pid)
{

    wallet->pid = pid;
}

pid_t wallet_get_pid(Wallet wallet)
{

    return wallet.pid;
}

int wallet_get_coins(Wallet wallet)
{

    return wallet.coins;
}

void wallet_inc_coins(Wallet *wallet)
{

    wallet->coins++;
}

void wallet_set_coins(Wallet *wallet, int coins)
{

    wallet->coins = coins;
}

Bloque bloque_init(int id, int target, Wallet *wallets)
{

    Bloque bloque;
    int i;

    bloque.id = id;
    bloque.target = target;
    bloque.solution = -1;
    bloque.winner = -1;
    bloque.votes = 0;
    bloque.positives = 0;
    bloque.flag = -1;

    /* Inicialización de las carteras */
    for (i = 0; i < MAX_MINEROS; i++)
    {
        bloque.wallets[i].pid = wallets[i].pid;
        bloque.wallets[i].coins = wallets[i].coins;
    }

    return bloque;
}

int bloque_set_solution(Bloque *bloque, int solution)
{

    if (bloque == NULL)
    {
        return -1;
    }

    bloque->solution = solution;

    return 0;
}

int bloque_set_winner(Bloque *bloque, pid_t winner)
{

    if (bloque == NULL)
    {
        return -1;
    }

    bloque->winner = winner;

    return 0;
}

int bloque_set_votes(Bloque *bloque, int votes)
{

    if (bloque == NULL)
    {
        return -1;
    }

    bloque->votes = votes;

    return 0;
}

int bloque_set_positives(Bloque *bloque, int positives)
{

    if (bloque == NULL)
    {
        return -1;
    }

    bloque->positives = positives;

    return 0;
}

int bloque_set_flag(Bloque *bloque, int flag)
{

    if (bloque == NULL)
    {
        return -1;
    }

    bloque->flag = flag;

    return 0;
}

int bloque_get_id(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->id;
}

int bloque_get_target(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->target;
}

int bloque_get_solution(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->solution;
}

pid_t bloque_get_winner(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->winner;
}

int bloque_get_votes(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->votes;
}

int bloque_get_positives(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->positives;
}

int bloque_get_flag(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return -1;
    }

    return bloque->flag;
}

Wallet *bloque_get_wallets(Bloque *bloque)
{

    if (bloque == NULL)
    {
        return NULL;
    }

    return bloque->wallets;
}
