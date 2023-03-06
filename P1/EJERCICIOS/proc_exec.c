/**
 * @file proc_exec.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Que sucede si se sustituye el primer elemento del array argv por la cadena "mi-ls"? ¿Por que?
 * 
 *   No hay ningún error, ya que el primer elemento del array argv es el nombre del programa.
 *   Por lo tanto, si se sustituye el primer elemento del array argv por la cadena "mi-ls",
 *   el programa se ejecuta correctamente. El problema vnedría si se cambia el primer argumento de execvp.
 * 
 * b) ¿Que modificaciones habr ́ıa que hacer en el programa anterior para utilizar la funcion
 * execl en lugar de execvp?
 * 
 *  Habría que cambiar el primer argumento de execvp por el path del programa, y pasar los argumentos
 *  de uno en uno en lugar de pasar un array de argumentos. El ultimo argumento deber ser NULL.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  char *argv[3] = {"ls", "./", NULL};
  pid_t pid;

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    if (execl("/usr/bin/ls", argv[0], argv[1], NULL)) {
      perror("execl");
      exit(EXIT_FAILURE);
    }
  } else {
    wait(NULL);
  }
  exit(EXIT_SUCCESS);
}