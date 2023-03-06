/**
 * @file pipe_example.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) Ejecutar el codigo. ¿Qu ́e se imprime por pantalla?
 * 
 *  I have received the string: Hi all!
    I have written in the pipe
 * 
 * b) ¿Que ocurre si el proceso padre no cierra el extremo de escritura? ¿Por que?
 * 
 * El proceso padre se queda bloqueado en la llamada a read() porque está esperando a que
 * haya algo que leer. No puede reconocer que la escritura ha terminado porque no ha cerrado
 * el extremo de escritura.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int fd[2];

  const char *string = "Hi all!\n";
  char readbuffer[80];
  int pipe_status;
  pid_t childpid;
  ssize_t nbytes;

  pipe_status = pipe(fd);
  if (pipe_status == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  childpid = fork();
  if (childpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (childpid == 0) {
    /* Close of the read end in the child. */
    close(fd[0]);
    /* The message is written in the write end. */
    /* strlen(string) + 1 < PIPE_BUF, there are no short writes. */
    nbytes = write(fd[1], string, strlen(string) + 1);
    if (nbytes == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    printf("I have written in the pipe\n");

    exit(EXIT_SUCCESS);
  } else {
    /* Close of the write end in the parent. */
    close(fd[1]);
    /* The message is read. */
    nbytes = 0;
    do {
      nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
      if (nbytes == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      if (nbytes > 0) {
        printf("I have received the string: %.*s", (int)nbytes, readbuffer);
      }
    } while (nbytes != 0);

    wait(NULL);
    exit(EXIT_SUCCESS);
  }
}