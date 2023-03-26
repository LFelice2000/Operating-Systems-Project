/**
 * @file sig_capture.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿La llamada a sigaction supone que se ejecute la funci ́on handler?
 * 
 * No, la llamada a sigaction no supone que se ejecute la función handler. La función
 * handler se ejecuta cuando se recibe la señal. En sigaction, sólo se le indica al
 * sistema operativo que cuando se reciba la señal, se ejecute la función handler.
 * 
 * 
 * b) ¿Se bloquea alguna se ̃nal durante la ejecuci ́on de la funci ́on handler?
 * 
 * No, no se bloquea ninguna señal durante la ejecución de la función handler. En la máscara
 * de señales, se indica que no se bloqueen señales. Por lo tanto, se pueden recibir señales
 * mientras se ejecuta la función handler.
 * 
 * 
 * c) ¿Cu ́ando aparece el printf en pantalla?
 * 
 * El printf aparece en pantalla cuando se recibe la señal SIGINT.
 * 
 * d) Modificar el programa anterior para que no capture SIGINT. ¿Qu ́e sucede cuando se
 * pulsa Ctrl + C ? En general, ¿qu ́e ocurre por defecto cuando un programa recibe
 * una se ̃nal y no tiene instalado un manejador?
 * 
 * Cuando se pulsa Ctrl + C, el programa termina. Por defecto, cuando un programa recibe una
 * señal y no tiene instalado un manejador, el programa termina. En este caso, el programa
 * termina porque recibe la señal SIGINT.
 * 
 * e) A partir del c ́odigo anterior, escribir un programa que capture todas las se ̃nales
 * (desde la 1 hasta la 31) usando el mismo manejador. ¿Se pueden capturar todas las
 * se ̃nales? ¿Por qu ́e?
 * 
 * No se pueden capturar todas las señales. Las señales SIGKILL y SIGSTOP no se pueden
 * capturar. Esto es porque estas señales no se pueden ignorar ni bloquear. Por lo tanto, no
 * se pueden capturar.
 * 
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/* Handler function for the signal SIGINT. */
void handler(int sig) {
  printf("Signal number %d received\n", sig);
  fflush(stdout);
}

int main(void) {

  struct sigaction act;

  act.sa_handler = handler;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;

  for(int i = 1; i <= 31; i++) {
    if (i == SIGKILL || i == SIGSTOP) {
      continue;
    }
    if (sigaction(i, &act, NULL) < 0) {
      perror("sigaction");
      exit(EXIT_FAILURE);
    }
  }

  while (1) {
    printf("Waiting Ctrl+C (PID = %d)\n", getpid());
    sleep(9999);
  }
}
