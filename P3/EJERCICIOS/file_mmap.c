/**
 * @file file_mmap.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Que sucede cuando se ejecuta varias veces el programa anterior? ¿Por que?
 * 
 * El contador se incrementa en 1 cada vez que se ejecuta el programa. Esto se debe a que
 * el fichero se mapea en memoria y se incrementa el valor de la variable apuntada por el
 * puntero.
 * 
 * b) ¿Se puede leer el contenido del fichero ((test_file.dat)) con un editor de texto? ¿Por
 * que?
 * 
 * No se puede leer el contenido del fichero con un editor de texto porque el fichero está
 * mapeado en memoria y no se puede acceder a él. La única forma de leer el contenido del
 * fichero es ejecutando el programa.
 * 
 */

/* READER */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_NAME "test_file.dat"

int main(void) {
  int created = 0;
  int fd;
  int *mapped = NULL;

  if ((fd = open(FILE_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
    if (errno == EEXIST) {
      if ((fd = open(FILE_NAME, O_RDWR, 0)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
      }
    } else {
      perror("open");
      exit(EXIT_FAILURE);
    }
  } else {
    if (ftruncate(fd, sizeof(int)) == -1) {
      perror("ftruncate");
      close(fd);
      exit(EXIT_FAILURE);
    }
    created = 1;
  }

  if ((mapped = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    close(fd);
    exit(EXIT_FAILURE);
  }
  close(fd);

  if (created) {
    *mapped = 0;
  }
  *mapped += 1;
  printf("Counter: %d\n", *mapped);
  munmap(mapped, sizeof(int));

  exit(EXIT_SUCCESS);
}