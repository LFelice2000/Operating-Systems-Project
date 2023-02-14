/**
 * @file proc_example.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) Analizar el texto que imprime el programa. ¿Se puede saber a priori en que orden
 * se imprimira el texto? ¿Por que?
 * 
 * b) Cambiar el codigo para que el proceso hijo imprima su PID y el de su padre en vez
 * de la variable i.
 * 
 * c) Dados los dos siguientes diagramas de ́arbol. ¿A cual de los dos arboles de procesos se corresponde el codigo de arriba, y por
 * que? ¿Que modificaciones habrıa que hacer en el codigo para obtener el otro ́arbol
 * de procesos?
 * 
 * d) El codigo original deja procesos huerfanos, ¿por que?
 * e) Introducir el mınimo n ́umero de cambios en el codigo para que no deje procesos
 * huerfanos.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 3

int main(void) {
  int i;
  pid_t pid;

  for (i = 0; i < NUM_PROC; i++) {
    pid = fork();
    if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      printf("Child pid: %d and Parent pid:%d\n", getpid(), getppid());
      exit(EXIT_SUCCESS);
    } else if (pid > 0) {
      printf("Parent %d\n", i);
    }
  }
  
  exit(EXIT_SUCCESS);
}