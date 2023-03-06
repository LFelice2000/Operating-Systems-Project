/**
 * @file file_descriptors.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * El programa se para en ciertos momentos para esperar a que el usuario pulse . Se pue-
 * den observar los descriptores de fichero del proceso en cualquiera de esos momentos si en
 * otra terminal se inspecciona el directorio /proc/<PID>/fd, donde <PID> es el identificador
 * del proceso. A continuaci ́on se indica qu ́e hacer en cada momento.
 * 
 * a) Stop 1. Inspeccionar los descriptores de fichero del proceso. ¿Qu ́e descriptores de
 * fichero se encuentran abiertos? ¿A qu ́e tipo de fichero apuntan?
 * 
 * Hay 14 descriptores de fichero abiertos, todos ellos apuntan a ficheros regulares.
 * 
 * b) Stop 2 y Stop 3. ¿Que cambios se han producido en la tabla de descriptores de
 * fichero?
 * 
 * Se han creado dos ficheros regulares, FILE1 y FILE2, y se han abierto dos descriptores
 * de fichero, uno para cada fichero (3 y 4)
 * 
 * c) Stop 4. ¿Se ha borrado de disco el fichero FILE1? ¿Por qu ́e? ¿Se sigue pudiendo ac-
 * ceder al fichero a trav ́es del directorio /proc? ¿Hay, por tanto, alguna forma sencilla
 * de recuperar los datos?
 * 
 * No se ha borrado de disco el fichero FILE1, porque no se ha cerrado el descriptor. Sólo
 * se ha hecho unlink, que elimina el enlace al fichero, pero no el fichero en sí. Sí se 
 * puede acceder al fichero y se puede leer, pero no se puede escribir.
 * 
 * 
 * d) Stop 5, Stop 6 y Stop 7. ¿Qu ́e cambios se han producido en la tabla de descriptores
 * de fichero? ¿Qu ́e se puede deducir sobre la numeraci ́on de un descriptor de fichero
 * obtenido tras una llamada a open?
 * 
 * Se han creado dos ficheros regulares, FILE3 y FILE4, y se han abierto dos descriptores.
 * Se había cerrado el 3, pero se ha vuelto a abrir con otro fichero, por lo que el 3 se ha vuelto a usar.
 * 
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MESSAGE "Hello"

#define FILE1 "file1.txt"
#define FILE2 "file2.txt"
#define FILE3 "file3.txt"

int main(void) {
  int file1, file2, file3, file4;
  size_t target_size;
  ssize_t size_written, total_size_written;

  fprintf(stderr, "PID = %d\nStop 1\n", getpid());
  getchar();

  if ((file1 = open(FILE1, O_CREAT | O_TRUNC | O_RDWR,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  /* Write message. */
  total_size_written = 0;
  target_size = sizeof(MESSAGE);
  do {
    size_written = write(file1, MESSAGE + total_size_written,
                         target_size - total_size_written);
    if (size_written == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    total_size_written += size_written;
  } while (total_size_written != target_size);

  fprintf(stderr, "Stop 2\n");
  getchar();

  if ((file2 = open(FILE2, O_CREAT | O_TRUNC | O_RDWR,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "Stop 3\n");
  getchar();

  if (unlink(FILE1) != 0) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "Stop 4\n");
  getchar();

  close(file1);

  fprintf(stderr, "Stop 5\n");
  getchar();

  if ((file3 = open(FILE3, O_CREAT | O_TRUNC | O_RDWR,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "Stop 6\n");
  getchar();

  if ((file4 = open(FILE3, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) ==
      -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "Stop 7\n");
  getchar();

  close(file2);
  close(file3);
  close(file4);

  if (unlink(FILE2) != 0) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }

  if (unlink(FILE3) != 0) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
