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
 *   El texto se imprime en orden aleatorio, ya que no se sabe en que orden se ejecutan los procesos.
 * 
 * b) Cambiar el codigo para que el proceso hijo imprima su PID y el de su padre en vez
 * de la variable i.
 * 
 *  El código se ha cambiado para que el proceso hijo imprima su PID y el de su padre.
 *  printf("Child: %d and Parent: %d\n", getpid(), getppid());
 * 
 * c) Dados los dos siguientes diagramas de ́arbol. ¿A cual de los dos arboles de procesos se corresponde el codigo de arriba, y por
 * que? ¿Que modificaciones habrıa que hacer en el codigo para obtener el otro ́arbol
 * de procesos?
 * 
 *  El código se corresponde con el primer árbol de procesos, ya que el padre crea 3 hijos.
 *  Para obtener el segundo árbol de procesos, habría que cambiar el código para que el padre no cree hijos,
 *  sino que el hijo cree uno nuevo. Mientras, cada padre debería de esperar a que el hijo termine.
 * 
 * d) El codigo original deja procesos huerfanos, ¿por que?
 * 
 * El código original deja procesos huerfanos, ya que el padre sólo recoge a uno de sus tres hijos.
 * 
 * 
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
      printf("Child: %d and Parent: %d\n", /*i*/ getpid(), getppid());
      fork();
      wait(NULL);
      exit(EXIT_SUCCESS);
    } else if (pid > 0) {
      printf("Parent %d\n", i);
      wait(NULL);
      exit(EXIT_SUCCESS);
    }
  }

  wait(NULL);
  exit(EXIT_SUCCESS);
}