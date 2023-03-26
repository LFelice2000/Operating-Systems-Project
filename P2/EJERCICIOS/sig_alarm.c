/**
 * @file sig_alarm.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Qu ́e sucede si, mientras se realiza la cuenta, se env ́ıa la se ̃nal SIGALRM al proceso?
 * 
 * Se ejecuta el manejador de la señal SIGALRM, handler_SIGALRM, que muestra un mensaje
 * y termina el proceso.
 * 
 * b) ¿Qu ́e sucede si se comenta la llamada a sigaction?
 * 
 * Se ejecutará el manejador por defecto de la señal SIGALRM, que imprime 'Alarm clock'
 * y termina el proceso. No se imprime el mensaje de 'End of program'.
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SECS 10

/* The handler shows a message and ends the process. */
void handler_SIGALRM(int sig) {
  printf("\nThese are the numbers I had time to count\n");
  exit(EXIT_SUCCESS);
}

int main(void) {
  struct sigaction act;
  long int i;

  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;

  /* The handler for SIGALRM is set. */
  /*act.sa_handler = handler_SIGALRM;
  if (sigaction(SIGALRM, &act, NULL) < 0) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }*/

  if (alarm(SECS)) {
    fprintf(stderr, "There is a previously established alarm\n");
  }

  fprintf(stdout, "The count starts (PID=%d)\n", getpid());
  for (i = 0;; i++) {
    fprintf(stdout, "%10ld\r", i);
    fflush(stdout);
  }

  fprintf(stdout, "End of program\n");
  exit(EXIT_SUCCESS);
}
