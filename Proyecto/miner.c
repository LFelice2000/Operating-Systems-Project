/**
 * @file miner.c
 * @author Luis Felice y Angela Valderrama
 * @brief 
 * @version 0.1
 * @date 2023-05-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>
#include <math.h>
#include <sys/mman.h>
#include <mqueue.h>

#include "miner_func.h"
#include "miner.h"
#include "pow.h"

#define SEM_NAME "/sem_minero"
#define SEM_WINNER "/sem_winner"
#define SEM_MQUEUE "/sem_mqueue"
#define SHM_NAME "/shm_sistema"

#define MQ_NAME "/mq_monitor"

int found = 0;
int target = 0;

int fd[2];

int got_SIGUSR1 = 0;
int got_SIGUSR2 = 0;

sem_t *sem_minero;
sem_t *sem_winner;
sem_t *sem_mqueue;

Sistema *sistema = NULL;

sigset_t set, oldset, intset;

mqd_t mq;

void handler(int sig){
    if(sig == SIGUSR1){
        got_SIGUSR1 = 1;
    }else if(sig == SIGUSR2){
        got_SIGUSR2 = 1;
    }else if(sig == SIGINT || sig == SIGALRM){
        if(sig == SIGINT){
            printf("Finishing by signal...\n");
        }
        clear();
        exit(EXIT_SUCCESS);        
    }
}

int main(int argc, char *argv[]){

    int n_seconds = 0, n_threads = 0;
    pid_t pid;

    /* Control de errores de los parámetros de entrada */
    if(argc < 3){
        printf("[ERROR] El programa debe iniciarse de la siguiente manera: ./miner <N_SECONDS> <N_THREADS>\n");
        exit(EXIT_FAILURE);
    }

    if(atoi(argv[1]) < 0){
        printf("[ERROR] El número de segundos debe ser mayor o igual que 0\n");
        exit(EXIT_FAILURE);
    }

    if(atoi(argv[2]) < 1){
        printf("[ERROR] El número de hilos debe ser mayor o igual que 1\n");
        exit(EXIT_FAILURE);
    }

    /* Obtener los argumentos */
    n_seconds = atoi(argv[1]);
    n_threads = atoi(argv[2]);

    /* Creación de la tubería */
    if(pipe(fd) == -1){
        printf("[ERROR] No se ha podido crear la tubería\n");
        exit(EXIT_FAILURE);
    }

    /* Creación del proceso Registrador*/
    pid = fork();
    if(pid == 0){
        registrador();
    }else if(pid > 0){
        minero(n_threads, n_seconds);
    }else{
        printf("[ERROR] No se ha podido crear el proceso Registrador\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}

void minero(int n_threads, int n_seconds){
    
    int first = 0;

    /* Cierre de los extremos de la tubería que no se van a usar */
    close(fd[0]);

    /* Configuración de las señales */
    signals(n_seconds);

    /* Creación o apertura del semáforo de los mineros */
    sem_minero = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(sem_minero == SEM_FAILED){
        printf("[ERROR] No se ha podido crear el semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Creación o apertura del semáforo del ganador */
    sem_winner = sem_open(SEM_WINNER, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(sem_winner == SEM_FAILED){
        printf("[ERROR] No se ha podido crear el semáforo\n");
        exit(EXIT_FAILURE);
    }

    sem_mqueue = sem_open(SEM_MQUEUE, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(sem_mqueue == SEM_FAILED){
        printf("[ERROR] No se ha podido crear el semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Conexión a memoria */
    if(sem_trywait(sem_minero) == 0){

        /* Soy el primer minero */
        first = 1;

        sistema = firstminer_setup();

        /* Configuración inicial de la estructura del segmento compartido */
        sistema_init(sistema);

    }else{
            
        /* No soy el primer minero */
        sistema = miner_setup();

    }

    /* Añadir el minero al sistema */
    sem_wait(&sistema->mutex);
    if(sistema_add_minero(sistema, getpid()) == -1){
        printf("[ERROR] No se ha podido registrar el minero\n");
        clear();
        exit(EXIT_FAILURE);
    }
    sem_post(&sistema->mutex);

    /* Configurar y abrir la cola de mensajes */
    mq = queue_setup();

    /* Preparación */
    if(first == 1){

        /* Configuración del bloque inicial */
        sistema_set_current_bloque(sistema, 0);

        /* Envío señal SIGUSR1 a los demás mineros */
        sem_wait(&sistema->mutex);
        send_signal(sistema, SIGUSR1);
        sem_post(&sistema->mutex);

        got_SIGUSR1 = 1;

    }else{

        /* Esperar a recibir la señal SIGUSR1 */
        while(got_SIGUSR1 == 0){
            sigsuspend(&oldset);
        }

        /* Obtener el bloque actual */
        sem_wait(&sistema->mutex);
        sistema_get_current_target(sistema, &target);
        sem_post(&sistema->mutex);

        got_SIGUSR2 = 0;

    }

    /* Bucle principal */
    while(got_SIGUSR1 == 1){

        got_SIGUSR1 = 0;

        /* Desbloquear SIGUSR2 para que pueda ser recibida */
        sigprocmask(SIG_UNBLOCK, &set, &oldset);

        /* Bloquear SIGINT y SIGALRM que están en intset */
        sigprocmask(SIG_BLOCK, &intset, &oldset);

        /* Minado hasta que sea el primero en encontrar la solución */
        minado(n_threads);

        found = 0;

        /* Desbloquear SIGINT y SIGALRM */
        sigprocmask(SIG_UNBLOCK, &intset, &oldset); 

        /* Bloquear SIGUSR1 y SIGUSR2 */
        sigprocmask(SIG_BLOCK, &set, &oldset);

        /* Intento de proclamación de ganador */
        if(sem_trywait(sem_winner) == 0){

            /* Soy el ganador */

            sem_wait(&sistema->mutex);

            got_SIGUSR2 = 0;

            voting_setup(sistema, target);

            sem_post(&sistema->mutex);

            /* Comprobar que todos los demás procesos han votado */
            check_voting(sistema);

            sem_wait(&sistema->mutex);

            /* Recuento de votos */
            count_votes(sistema);

            /* Guardamos este bloque como el último resuelto */
            sistema_set_last(sistema);

            /* Enviar bloque resulto a Monitor mediante cola de mensajes */
            /* Sincronizar con semáforos con nombre */
            
            if(sem_trywait(sem_mqueue) == 0){
                send_to_monitor(sistema, mq);
            }

            /* Preparar bloque para nueva ronda */
            sistema_set_current_bloque(sistema, target);

            /* Envío señal SIGUSR1 a los demás mineros */
            send_signal(sistema, SIGUSR1);

            sem_post(&sistema->mutex);

            /* Liberar el semáforo winner */
            sem_post(sem_winner);

            sem_wait(&sistema->mutex);

            /* Enviar último bloque resuelto al Registrador */
            send_to_registrador(sistema, fd);

            sem_post(&sistema->mutex);

            got_SIGUSR1 = 1;


        }else{

            /* No soy el ganador */

            /* Esperar a recibir la señal SIGUSR2 */
            while(got_SIGUSR2 == 0){
                sigsuspend(&oldset);
            }

            got_SIGUSR2 = 0;

            /* Votar */
            sem_wait(&sistema->mutex);
            vote(sistema);
            sem_post(&sistema->mutex);

            while(got_SIGUSR1 == 0){
                sigsuspend(&oldset);
            }

            /* Enviar último bloque resuelto al Registrador */
            sem_wait(&sistema->mutex);

            send_to_registrador(sistema, fd);
            sistema_get_current_target(sistema, &target);

            sem_post(&sistema->mutex);

        }
                
    }

}

void registrador(){

    ssize_t nbytes;
    Bloque bloque;
    Wallet *wallets;
    char filename[20];
    int i, file = 0, positives, total;

    /* Cierre del extremo de escritura del pipe */
    close(fd[1]);

    /* Creación del archivo de registro */
    sprintf(filename, "%d.txt", getppid());

    file = open(filename, O_RDWR | O_CREAT | O_EXCL , S_IRUSR | S_IWUSR);
    if(file == -1){
        perror("[ERROR] No se ha podido crear el archivo de registro\n");
        exit(EXIT_FAILURE);
    }
    
    do{

        nbytes = read(fd[0], &bloque, sizeof(Bloque));
        if(nbytes == -1){
            perror("[ERROR] No se ha podido leer el bloque del pipe\n");
            exit(EXIT_FAILURE);
        }

        if(nbytes > 0){

            dprintf(file, "Id:       %04d\n", bloque_get_id(&bloque));
            dprintf(file, "Winner:   %d\n", bloque_get_winner(&bloque));
            dprintf(file, "Target:   %08d\n", bloque_get_target(&bloque));
            dprintf(file, "Solution: %08d", bloque_get_solution(&bloque));
            positives = bloque_get_positives(&bloque);
            total = bloque_get_votes(&bloque);
            if(positives >= total/2){
                dprintf(file, " (validated)\n");
            }else{
                dprintf(file, " (rejected)\n");
            }
            dprintf(file, "Votes:    %d/%d\n", positives, total);
            dprintf(file, "Wallets:  ");
            wallets = bloque_get_wallets(&bloque);
            for(i = 0; i < MAX_MINEROS; i++){
                if(wallet_get_pid(wallets[i]) != -1){
                    dprintf(file, "%d:%02d ", wallet_get_pid(wallets[i]), wallet_get_coins(wallets[i]));
                }
            }
            dprintf(file, "\n\n");
        }

    } while(nbytes != 0);

    close(file);
    close(fd[0]);

    exit(EXIT_SUCCESS);

}

void signals(int n_seconds){

    struct sigaction act;

    /* Configuración de las señales */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    /* Bloqueo de las señales */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    sigemptyset(&intset);
    sigaddset(&intset, SIGINT);
    sigaddset(&intset, SIGALRM);

    sigprocmask(SIG_BLOCK, &set, &oldset);

    if(sigaction(SIGUSR1, &act, NULL) == -1){
        printf("[ERROR] No se ha podido configurar la señal SIGUSR1\n");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGUSR2, &act, NULL) == -1){
        printf("[ERROR] No se ha podido configurar la señal SIGUSR2\n");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGINT, &act, NULL) == -1){
        printf("[ERROR] No se ha podido configurar la señal SIGINT\n");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGALRM, &act, NULL) == -1){
        printf("[ERROR] No se ha podido configurar la señal SIGALRM\n");
        exit(EXIT_FAILURE);
    }

    if(alarm(n_seconds) == -1){
        printf("[ERROR] No se ha podido configurar el temporizador\n");
        exit(EXIT_FAILURE);
    }

}

void minado(int n_threads){

    pthread_t *threads = NULL;
    Info *thInfo = NULL;
    int i, err, init, end;

    /* Se crea un array para guardar los identificadores de los hilos */
    threads = (pthread_t*)malloc(n_threads*sizeof(pthread_t));
    if(threads == NULL) {
        fprintf(stderr, "Error: no se ha podido reservar memoria para los identificadores de los hilos.\n");
        return;
    }

    /* Se crea un array para guardar la información de los hilos */
    thInfo = (Info*)malloc(n_threads*sizeof(Info));
    if(thInfo == NULL) {
        fprintf(stderr, "Error: no se ha podido reservar memoria para la información de los hilos.\n");
        free(threads);
        return;
    }

    /* Se calcula el número de iteraciones que realizará cada hilo */
    init = (int)floor((double)(POW_LIMIT)/n_threads)+1;
    end = init;
    
    /* Se crean y se lanzan los hilos que ejecutarán la prueba de fuerza */
    for(i = 0; i < n_threads; i++) {
        
        /* Se calcula el rango de búsqueda de cada hilo */
        thInfo[i].lower = init * i;
        thInfo[i].upper = end - 1;

        /* Se comprueba que el rango de iteraciones no se salga del límite */
        end += init;
        if(end > POW_LIMIT) {
            end = POW_LIMIT;
        }

        /* Se crea el hilo */
        err = pthread_create(&threads[i], NULL, prueba_de_fuerza, (void *)&thInfo[i]);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            free(threads);
            return;
        }
    
    }

    /* Se espera a que terminen los hilos*/
    for(i = 0; i < n_threads; i++) {

        err = pthread_join(threads[i], NULL);
        if(err != 0) {
            fprintf(stderr, "pthread_join : %s\n", strerror(err));
            free(threads);
            return;
        }

    }

    /* Se libera la memoria */
    free(threads);
    free(thInfo);

    return;
    
}

void *prueba_de_fuerza(void *info) {

    int res = 0, i = 0;
    Info *thInfo = (Info*)info;

    /* Se prueba la fuerza bruta */
    i = thInfo->lower;
    while(found == 0 && got_SIGUSR2 == 0){

        /* Se comprueba que el número no se salga del rango */
        if(i > thInfo->upper) {
            break;
        }

        /* Se calcula el hash */
        res = pow_hash(i);

        /* Se comprueba si el hash es igual al objetivo */
        if(res == target) {
            
            /* Se guarda la solucion en la variable global target y 
                se cambia la flag para que los demás hilos dejen de minar */
            found = 1;
            target = i;

            return NULL;
        }

        i++;

    }
    
    return NULL;
}

void clear(){

    int last = 0;

    /* Desregistrarse del sistema */
    sem_wait(&sistema->mutex);
    sistema_remove_minero(sistema, getpid());
    
    if(sistema_get_nmineros(sistema) == 0){
        last = 1;
    }
    sem_post(&sistema->mutex);

    /* Si es el último minero, se destruyen los semáforos */
    if(last == 1){
        sem_wait(&sistema->mutex);

        /* Enviar bloque de finalización al Monitor */
        sistema_set_current_bloque(sistema, -1);

        if(sem_trywait(sem_mqueue) == 0){
            send_to_monitor(sistema, mq);
        }

        sem_post(&sistema->mutex);

        /* Destruir los semáforos */
        sistema_destroy(sistema);

        /* Unmap de la memoria compartida */
        munmap(sistema, sizeof(Sistema));

        /* Cerrar la cola de mensajes */
        mq_close(mq);

        /* Eliminar la cola de mensajes */
        mq_unlink(MQ_NAME);

        sem_close(sem_minero);
        sem_close(sem_winner);
        sem_close(sem_mqueue);

        /* Eliminar el segmento de memoria */
        shm_unlink(SHM_NAME);

        /* Eliminar el semáforo de los mineros */
        sem_unlink(SEM_NAME);

        /* Eliminar el semáforo del ganador */
        sem_unlink(SEM_WINNER);

        /* Eliminar el semáforo de la cola de mensajes */
        sem_unlink(SEM_MQUEUE);

        /* Cerrar pipe con el proceso Registrador */
        close(fd[1]);
    }

    sem_close(sem_minero);
    sem_close(sem_winner);
    sem_close(sem_mqueue);

    /* Cerrar pipe con el proceso Registrador */
    close(fd[0]);

    /* Cerrar cola de mensajes */
    mq_close(mq);

}

