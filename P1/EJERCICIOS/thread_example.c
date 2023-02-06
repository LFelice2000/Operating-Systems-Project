/**
 * @file thread_example.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Que hubiera pasado si el proceso no hubiera esperado a los hilos? Para probarlo
 * basta eliminar las llamadas a pthread_join.
 * 
 * b) Con el codigo modificado del apartado anterior, indicar que ocurre si se reemplaza
 * la funcion exit por una llamada a pthread_exit
 * 
 * c) Tras eliminar las llamadas a pthread_join en los apartados anteriores, el programa
 * no espera a que terminen todos los hilos. ¿Que codigo habrıa que anadir para que
 * los recursos del sistema operativo se gestionen correctamente a pesar de que no se
 * espere a los hilos creados?
 * 
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *slow_printf(void *arg) {
  const char *msg = arg;
  size_t i;

  for (i = 0; i < strlen(msg); i++) {
    printf("%c ", msg[i]);
    fflush(stdout);
    sleep(1);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t h1;
  pthread_t h2;
  char *hello = "Hello ";
  char *world = "World";
  int error;

  error = pthread_create(&h1, NULL, slow_printf, hello);
  if (error != 0) {
    fprintf(stderr, "pthread_create: %s\n", strerror(error));
    exit(EXIT_FAILURE);
  }

  error = pthread_create(&h2, NULL, slow_printf, world);
  if (error != 0) {
    fprintf(stderr, "pthread_create: %s\n", strerror(error));
    exit(EXIT_FAILURE);
  }

  error = pthread_join(h1, NULL);
  if (error != 0) {
    fprintf(stderr, "pthread_join: %s\n", strerror(error));
    exit(EXIT_FAILURE);
  }

  error = pthread_join(h2, NULL);
  if (error != 0) {
    fprintf(stderr, "pthread_join: %s\n", strerror(error));
    exit(EXIT_FAILURE);
  }

  printf("Program %s finished correctly\n", argv[0]);
  exit(EXIT_SUCCESS);
}
