/**
 * @file sig_capture_improved.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) En esta versi ́on mejorada del programa del Ejercicio 3, ¿en qu ́e l ́ıneas se realiza
 * realmente la gesti ́on de la se ̃nal?
 * 
 * Dentro del bucle while, se comprueba si se ha recibido la señal. Si se ha
 * recibido, se ejecuta el código que se encuentra dentro del if.
 * 
 * b) ¿Por qu ́e, en este caso, se permite el uso de variables globales?
 * 
 * Porque de esta manera, se puede acceder a la variable got_signal desde
 * cualquier parte del programa, teniendo en cuenta que la llamada a la función
 * handler es asíncrona.
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static volatile sig_atomic_t got_signal = 0;

/* Handler function for the signal SIGINT. */
void handler(int sig) { got_signal = 1; }

int main(void) {
  struct sigaction act;

  act.sa_handler = handler;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;

  if (sigaction(SIGINT, &act, NULL) < 0) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  while (1) {
    printf("Waiting Ctrl+C (PID = %d)\n", getpid());
    if (got_signal) {
      got_signal = 0;
      printf("Signal received.\n");
    }
    sleep(9999);
  }
}
