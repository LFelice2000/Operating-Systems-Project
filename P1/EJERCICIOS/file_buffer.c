/**
 * @file file_buffer.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Cuantas veces se escribe el mensaje ((I am your father)) por pantalla? ¿Por que?
 * 
 * b) En el programa falta el terminador de l ́ınea (\n) en los mensajes. Corregir este
 * problema. ¿Sigue ocurriendo lo mismo? ¿Por qu ́e?
 * 
 * c) Ejecutar el programa redirigiendo la salida a un fichero. ¿Que ocurre ahora? ¿Porque?
 * 
 * d) ¿Como se puede corregir definitivamente este problema sin dejar de usar printf?
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  pid_t pid;

  printf("I am your father");

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    printf("Noooooo");
    exit(EXIT_SUCCESS);
  }

  wait(NULL);
  exit(EXIT_SUCCESS);
}
