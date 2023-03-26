/**
 * @file sig_sigset.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Qu ́e sucede cuando el programa anterior recibe SIGUSR1 o SIGUSR2? ¿Y cuando
 * recibe SIGINT?
 * 
 * Cuando el programa recibe SIGUSR1 o SIGUSR2, no pasa nada porque las dos señales
 * están bloqueadas. Cuando recibe SIGINT, se termina el programa.
 * 
 * b) Modificar el programa anterior para que, en lugar de hacer una llamada a pause,
 * haga una llamada a sleep para suspenderse durante 10 segundos, tras la que debe
 * restaurar la m ́ascara original. Ejecutar el programa, y durante los 10 segundos de
 * espera, enviarle SIGUSR1. ¿Qu ́e sucede cuando finaliza la espera? ¿Se imprime el
 * mensaje de despedida? ¿Por qu ́e?
 * 
 * Cuando finaliza la espera, se imprime el mensaje de despedida. Esto es porque, al
 * terminar la espera, se restaura la máscara original. En la máscara original, no se
 * bloquea ninguna señal. Por lo tanto, se pueden recibir señales. En este caso, se
 * recibe la señal SIGUSR1 y se termina el programa porque es su rutina por defecto.
 * Si no se recibiera la señal SIGUSR1, se imprimiría el mensaje de despedida.
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  sigset_t set, oset;

  /* Mask to block signals SIGUSR1 and SIGUSR2. */
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);

  /* Blocking of signals SIGUSR1 and SIGUSR2 in the process. */
  if (sigprocmask(SIG_BLOCK, &set, &oset) < 0) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("Waiting signals (PID = %d)\n", getpid());
  printf("SIGUSR1 and SIGUSR2 are blocked\n");
  sleep(10);

  /* Restore the original mask. */
  if (sigprocmask(SIG_SETMASK, &oset, NULL) < 0) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  printf("End of program\n");
  exit(EXIT_SUCCESS);
}
