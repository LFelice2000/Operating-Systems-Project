#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include "miner.h"
#include "pow.h"

#define SEM_NAME "/sem_minero"
#define SHM_NAME "/shm_minero"

int solucion = 0;
int target = 0;

int got_SIGUSR1 = 0;
int got_SIGUSR2 = 0;

sem_t *sem_minero;

sigset_t set, oldset;
struct sigaction act;

void handler(int sig){
    if(sig == SIGUSR1){
        got_SIGUSR1 = 1;
    }else if(sig == SIGUSR2){
        got_SIGUSR2 = 1;
    }
}

int main(int argc, char *argv[]){

    int n_seconds = 0, n_threads = 0, fd[2];
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

    /* Configuración de las señales */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    /* Bloqueo de las señales */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    sigprocmask(SIG_BLOCK, &set, &oldset);

    /* Creación de la tubería */
    if(pipe(fd) == -1){
        printf("[ERROR] No se ha podido crear la tubería\n");
        exit(EXIT_FAILURE);
    }

    /* Creación del proceso Registrador*/
    pid = fork()
    if(pid == 0){
        registrador();
    }else if(pid > 0){
        minero(n_threads);
    }else{
        printf("[ERROR] No se ha podido crear el proceso Registrador\n");
        exit(EXIT_FAILURE);
    }

    

}

void minero(int n_threads){
    
    Sistema *sistema;
    Minero *minero;
    int fd_shm, value, i, j;
    struct stat statshm;

    if((minero = minero_init(n_threads)) == NULL){
        printf("[ERROR] No se ha podido inicializar la estructura Minero\n");
        exit(EXIT_FAILURE);
    }

    /* Creación o apertura del semáforo de los mineros */
    sem_minero = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(sem_minero == SEM_FAILED){
        printf("[ERROR] No se ha podido crear el semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Identificar el primer minero que llega */
    if(sem_trywait(sem_minero) == 0){

        /* Soy el primer minero */

        /* Creación el segmento de memoria compartida */
        fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if(fd_shm == -1){
            perror("[ERROR] No se ha podido crear el segmento de memoria compartida\n");
            exit(EXIT_FAILURE);
        }

        /* Establecer tamaño del segmento */
        if(ftruncate(fd_shm, sizeof(Sistema)) == -1){
            perror("[ERROR] No se ha podido establecer el tamaño del segmento de memoria compartida\n");
            exit(EXIT_FAILURE);
        }

        /* Mapeo del segmento de memoria */
        sistema = mmap(NULL, sizeof(Sistema), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
        close(fd_shm);
        if(sistema == MAP_FAILED){
            perror("[ERROR] No se ha podido mapear el segmento de memoria compartida\n");
            exit(EXIT_FAILURE);
        }

        /* Configuración inicial de la estructura del segmento compartido */
        sistema_init(sistema);

        /* Registro del minero con su PID en el sistema */
        sem_wait(&sistema->mutex);

        /* Comprobar el número de mineros */
        if(sistema->n_mineros == MAX_MINEROS){
            printf("[ERROR] El número máximo de mineros ha sido alcanzado\n");
            /* LIMPIAR RECURSOS */
            exit(EXIT_FAILURE);
        }

        /* Registro del minero con su PID en el sistema */
        /* Atravesamos el array por si un minero en la posición 7 se va mientras que el último
            registrado está en la posición 15 */
        for(i = 0; i < MAX_MINEROS; i++){
            if(sistema->pids[i] == -1){
                sistema->pids[i] = getpid();
                break;
            }
        }
        sistema->n_mineros++;
        
        sem_post(&sistema->mutex);

        /* Objetivo principal a 0 */
        sem_wait(&sistema->mutex);
        bloque_init(&sistema->current);
        sem_post(&sistema->mutex);

        /* Envío señal SIGUSR1 a los demás mineros */
        sem_wait(&sistema->mutex);
        for(i = 0; i < MAX_MINEROS; i++){
            if(sistema->pids[i] != -1 && sistema->pids[i] != getpid()){
                kill(sistema->pids[i], SIGUSR1);
            }
        }
        sem_post(&sistema->mutex);

    }else{
            
        /* No soy el primer minero */

        /* Abrir el segmento de memoria compartida */
        fd_shm = shm_open(SHM_NAME, O_RDWR, 0);

        /* Mapeo del segmento de memoria */
        sistema = mmap(NULL, sizeof(Sistema), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
        if(sistema == MAP_FAILED){
            perror("[ERROR] No se ha podido mapear el segmento de memoria compartida\n");
            exit(EXIT_FAILURE);
        }

        /* Comprobar el tamaño del segmento o el último semáforo */
        fstat(fd_shm, &statshm);
        sem_getvalue(&sistema->winner, &value);
        while(statshm.st_size != sizeof(Sistema) && value != 0){
            fstat(fd_shm, &statshm);
            sem_getvalue(&sistema->winner, &value);
        }

        /* Registro del minero con su PID en el sistema */
        sem_wait(&sistema->mutex);

        /* Comprobar el número de mineros */
        if(sistema->n_mineros == MAX_MINEROS){
            printf("[ERROR] El número máximo de mineros ha sido alcanzado\n");
            /* LIMPIAR RECURSOS */
            exit(EXIT_FAILURE);
        }

        /* Registro del minero con su PID en el sistema */
        /* Atravesamos el array por si un minero en la posición 7 se va mientras que el último
            registrado está en la posición 15 */
        for(i = 0; i < MAX_MINEROS; i++){
            if(sistema->pids[i] == -1){
                sistema->pids[i] = getpid();
                break;
            }
        }
        sistema->n_mineros++;
        
        sem_post(&sistema->mutex);

        /* Esperar a recibir la señal SIGUSR1 */
        while(got_SIGUSR1 == 0){
            sigsuspend(&oldset);
        }

    }

    /* Bucle principal */

    /* Minado hasta que sea el primero en encontrar la solución */
    while(got_SIGUSR2 == 0){
        solution = minado(n_threads, sistema, minero);
    }

    /* Intento de proclamación de ganador */
    if(sem_trywait(&sistema->winner) == 0){

        /* Soy el ganador */

        /* Actualización del bloque */
        sem_wait(&sistema->mutex);

        sistema->current.solution = solution;
        sistema->winner = getpid();
        /* Inicialización de los votos a -1 */
        for(i = 0; i < MAX_MINEROS; i++){
            sistema->votes[i] = -1;
        }

        sem_post(&sistema->mutex);

        /* Envío señal SIGUSR2 a los demás mineros */
        sem_wait(&sistema->mutex);
        for(i = 0; i < MAX_MINEROS; i++){
            if(sistema->pids[i] != -1 && sistema->pids[i] != getpid()){
                kill(sistema->pids[i], SIGUSR2);
            }
        }
        sem_post(&sistema->mutex);

        /* Comprobar que todos los demás procesos han votado */
        j = 0;
        while(j < MAX_MINEROS){
            sem_wait(&sistema->mutex);
            for(i = 0; i < MAX_MINEROS; i++){
                if(sistema->votes[i] != -1){
                    count++;
                }
            }

            if(count == sistema->n_mineros - 1){
                sem_post(&sistema->mutex);
                count = 0;
                break;
            }

            count = 0;

            sem_post(&sistema->mutex);
            if(i == MAX_MINEROS){
                break;
            }

            usleep(500);
            j++;
        }

        /* Recuento de votos */
        sem_wait(&sistema->mutex);
        for(i = 0; i < MAX_MINEROS; i++){
            if(sistema->votes[i] == 1){
                yes++;
            }else if(sistema->votes[i] == 0){
                no++;
            }
        }

        /* Si se recibe la aprobación, actualizar Bloque */
        if(yes > no){
            sistema->current.votes = yes + no;
            sistema->current.positives = yes;
        }
    
    }else{
            
        /* No soy el ganador */

        /* Esperar a recibir la señal SIGUSR2 */
        while(got_SIGUSR2 == 0){
            sigsuspend(&oldset);
        }

        /* LIMPIAR RECURSOS */
        exit(EXIT_SUCCESS);

    }

}

void registrador(){

}

Minero *minero_init(int n_threads){

    Minero *minero;

    minero = (Minero *)malloc(sizeof(Minero));
    if(minero == NULL){
        perror("[ERROR] No se ha podido reservar memoria para el minero\n");
        exit(EXIT_FAILURE);
    }

    minero->threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
    if(minero->threads == NULL){
        perror("[ERROR] No se ha podido reservar memoria para los hilos del minero\n");
        exit(EXIT_FAILURE);
    }

    return minero;

}

void sistema_init(Sistema *sistema){

    int i;

    /* Inicialización del número de mineros a 0 */
    sistema->n_mineros = 0;

    /* Inicialización de los pids a -1 */
    for(i = 0; i < MAX_MINEROS; i++){
        sistema->pids[i] = -1;
    }

    /* Inicialización de los votos a -1 */
    for(i = 0; i < MAX_MINEROS; i++){
        sistema->votes[i] = -1;
    }

    /* Inicialización de las monedas a 0 */
    for(i = 0; i < MAX_MINEROS; i++){
        sistema->coins[i] = 0;
    }

    /* Inicialización de los semáforos */    
    if(sem_init(&sistema->winner, 1, 1) == -1){
        perror("[ERROR] No se ha podido inicializar el semáforo winner\n");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&sistema->mutex, 1, 1) == -1){
        perror("[ERROR] No se ha podido inicializar el semáforo mutex\n");
        exit(EXIT_FAILURE);
    }

}

void bloque_init(Bloque *bloque){

    bloque->id = 0;
    bloque->target = 0;
    bloque->solution = -1;
    bloque->winner = -1;
    bloque->votes = 0;
    bloque->positives = 0;

    /* Inicialización de las carteras */
    for(int i = 0; i < MAX_MINEROS; i++){
        bloque->wallets[i].pid = -1;
        bloque->wallets[i].coins = 0;
    }

}

int minado(int n_threads, Sistema *sistema, Minero *minero){

    pthread_t *hilos = NULL;
    Info *thInfo = NULL;
    int i, err, init, end;

    /* Se crea un array para guardar la información de los hilos */
    thInfo = (Info*)malloc(n_threads*sizeof(Info));
    if(thInfo == NULL) {
        fprintf(stderr, "Error: no se ha podido reservar memoria para la información de los hilos.\n");
        free(hilos);
        return;
    }

    thInfo->sistema = sistema;

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
        err = pthread_create(&minero->threads[i], NULL, prueba_de_fuerza, (void *)&thInfo[i]);
        if(err != 0) {
            fprintf(stderr, "pthread_create : %s\n", strerror(err));
            free(hilos);
            return;
        }
    
    }

    /* Se espera a que terminen los hilos*/
    for(i = 0; i < n_threads; i++) {

        err = pthread_join(minero->threads[i], NULL);
        if(err != 0) {
            fprintf(stderr, "pthread_join : %s\n", strerror(err));
            free(hilos);
            return;
        }

    }

    /* Se libera la memoria */
    free(minero->threads);
    free(thInfo);

    return target;
    
}

void *prueba_de_fuerza(void *info) {

    int res = 0, i = 0;
    Info *thInfo = (Info*)info;

    /* Se prueba la fuerza bruta */
    i = thInfo->lower;
    while(got_SIGUSR2 == 0 || solucion == 0){

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
            solucion = 1;
            got_SIGUSR2 = 1;
            target = i;

            return NULL;
        }

        i++;
    }
    
    return NULL;
}

