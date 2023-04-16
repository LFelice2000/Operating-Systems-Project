/**
 * @file file_truncate.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * 
 * a) Completar el codigo anterior para obtener el tama ̃no del fichero abierto.
 * 
 * El tamaño del fichero se obtiene con la función fstat, que devuelve un struct stat
 * con la información del fichero. El tamaño del fichero se encuentra en el campo st_size
 * del struct stat. El tamaño es 12. 
 * 
 * b) Completar el codigo anterior para truncar el tamano del fichero a 5 B. ¿Qu ́e contiene
 * el fichero resultante?
 * 
 * El fichero resultante contiene el mensaje "Test message" truncado a 5 bytes, con lo cual, 
 * el mensaje queda "Test ".
 * 
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/shm_example"
#define MESSAGE "Test message"

int main(int argc, char *argv[]) {
  int fd;
  struct stat statbuf;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <FILE>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  fd = open(argv[1], O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  dprintf(fd, "%s", MESSAGE);
  /* Get size of the file. */

  fstat(fd, &statbuf);
  printf("[BEFORE]: Size of the file: %ld\n", statbuf.st_size);

  /* Truncate the file to size 5. */
  ftruncate(fd, 5);
  fstat(fd, &statbuf);
  printf("[AFTER]: Size of the file: %ld\n", statbuf.st_size);

  close(fd);
  exit(EXIT_SUCCESS);
}
