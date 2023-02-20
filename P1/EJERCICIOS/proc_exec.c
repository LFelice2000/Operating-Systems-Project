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
 * b) ¿Que modificaciones habr ́ıa que hacer en el programa anterior para utilizar la funcion
 * execl en lugar de execvp?
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
    if (execl("ls", argv[1], argv[2])) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  } else {
    wait(NULL);
  }
  exit(EXIT_SUCCESS);
}