/**
 * @file proc_malloc.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) En el programa anterior se reserva memoria en el proceso padre y se inicializa en
 * el proceso hijo usando la funci ́on strcpy (que copia una cadena a una posici ́on de
 * memoria). Una vez el proceso hijo termina, el padre lo imprime por pantalla. ¿Qu ́e
 * ocurre cuando se ejecuta el c ́odigo? ¿Es este programa correcto? ¿Por qu ́e?
 * 
 *   El programa no se ejecuta correctamente, porque los dos procesos no comparten memoria
 *   y por lo tanto no se imprime el mensaje "Hello". Son procesos independientes.
 * 
 *   Para que funcione habría que hacer el strcpy antes de hacer la llamada a fork() y así
 *   los dos procesos tendrían acceso al mismo valor de la variable sentence.
 * 
 * b) El programa anterior contiene una fuga de memoria ya que el array sentence nunca
 * se libera. Corregir el codigo para eliminar esta fuga. ¿Donde hay que liberar la
 * memoria, en el proceso padre, en el hijo o en ambos? ¿Por que?
 * 
 *  Habría que liberar la memoria en el proceso padre, ya que es el que la reserva.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MESSAGE "Hello"

int main(void) {
  pid_t pid;
  char *sentence = calloc(sizeof(MESSAGE), 1);

  strcpy(sentence, MESSAGE);
  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    //strcpy(sentence, MESSAGE);
    exit(EXIT_SUCCESS);
  } else {
    wait(NULL);
    printf("Parent: %s\n", sentence);
    free(sentence);
    exit(EXIT_SUCCESS);
  }
}
