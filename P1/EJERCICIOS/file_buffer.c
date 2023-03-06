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
 *    Se escribe dos veces porque el mensaje sigue en el buffer de salida y no se ha
 *   vaciado.
 * 
 * b) En el programa falta el terminador de l ́ınea (\n) en los mensajes. Corregir este
 * problema. ¿Sigue ocurriendo lo mismo? ¿Por qu ́e?
 * 
 *   No, ya no ocurre lo mismo porque al escribir \n se vacía el buffer de salida.
 * 
 * c) Ejecutar el programa redirigiendo la salida a un fichero. ¿Que ocurre ahora? ¿Porque?
 * 
 *   Ocurre lo mismo que en el apartado a) porque el buffer de salida no se vacía.
 * 
 * d) ¿Como se puede corregir definitivamente este problema sin dejar de usar printf?
 * 
 *  Usando fflush() para vaciar el buffer de salida después de cada printf().
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  FILE *pf = NULL;
  pid_t pid;

  /*
  pf = fopen("file_buffer.txt", "w");
  if (pf == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  */

  /*
  fprintf(pf, "I am your father\n");
  fflush(pf);
  */
  printf("I am your father");
  fflush(stdout);

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    /*
    printf(pf, "Noooooo\n");
    fflush(pf);
    */
    printf("Noooooo");
    fflush(stdout);
    exit(EXIT_SUCCESS);
  }

  wait(NULL);
  //fclose(pf);
  exit(EXIT_SUCCESS);
}
