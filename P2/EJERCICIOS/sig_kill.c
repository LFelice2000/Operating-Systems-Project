/**
 * @file sig_kill.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * 
 * b) Probar el programa enviando la se ̃nal SIGSTOP de una terminal a otra (cuyo PID se
 * puede averiguar f ́acilmente con el comando ps). ¿Qu ́e sucede si se intenta escribir en
 * la terminal a la que se ha enviado la se ̃nal? ¿Y despu ́es de enviarle la se ̃nal SIGCONT?
 * 
 * Cuando se envia la señal SIGSTOP a una terminal, esta deja de recibir señales, por lo que
 * no se puede escribir en ella. Cuando se envia la señal SIGCONT, la terminal vuelve a
 * recibir señales y se puede escribir en ella. Lo que se había escrito después de enviar la
 * señal SIGSTOP, se puede ver al enviar la señal SIGCONT.
 * 
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int sig;
  pid_t pid;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s -<signal> <pid>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  sig = atoi(argv[1] + 1);
  pid = (pid_t)atoi(argv[2]);

  /* Complete the code. */
  if (kill(pid, sig) == -1) {
    perror("kill");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
