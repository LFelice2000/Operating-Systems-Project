/**
 * @file sem_signal.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Qu ́e sucede cuando se env ́ıa la se ̃nal SIGINT? ¿La llamada a sem_wait se ejecuta
 * con  ́exito? ¿Por qu ́e?
 * 
 * La llamada a sem_wait se ejecuta con éxito, porque el proceso queda bloqueado hasta
 * que se reciba la señal SIGINT, que es capturada por el manejador vacío, que no hace
 * nada. Por lo tanto, el proceso no termina.
 * 
 * b) ¿Qu ́e sucede si, en lugar de usar un manejador vac ́ıo, se ignora la se ̃nal con SIG_IGN?
 * 
 * Si se ignora la señal con SIG_IGN, el proceso no captura la señal, por lo que el proceso
 * sigue bloqueado en la llamada a sem_wait. Por lo tanto, el proceso no termina.
 * 
 * c) ¿Qu ́e cambios habr ́ıa que hacer en el programa anterior para garantizar que no
 * termine salvo que se consiga hacer el Down del sem ́aforo, lleguen o no se ̃nales cap-
 * turadas?
 * 
 * Para garantizar que no termine salvo que se consiga hacer el Down del semáforo, habría
 * que hacer que el proceso intentara hacer el Down del semáforo en un bucle, hasta que
 * se consiga hacer el Down del semáforo. Para ello, se podría usar la función sem_trywait que
 * intenta hacer el Down del semáforo, pero no bloquea al proceso si no se puede hacer el
 * Down del semáforo.
 * 
 */

#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_NAME "/example_sem"

void handler(int sig) { return; }

int main(void) {
  sem_t *sem = NULL;
  struct sigaction act;

  if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) ==
      SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;

  /* The handler for SIGINT is set. */
  act.sa_handler = handler;
  if (sigaction(SIGINT, &act, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  printf("Starting wait (PID=%d)\n", getpid());

  while(sem_trywait(sem) == -1) {
    printf("No se puede hacer Down...\n");
  }

  printf("Finishing wait\n");
  sem_unlink(SEM_NAME);
  
}
