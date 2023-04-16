/**
 * @file mq_send_receive.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿En que orden se envıan los mensajes y en que orden se reciben? ¿Por que?
 * 
 * Los mensajes se envían en el orden en el que se escriben en el código. Los mensajes se reciben según su prioridad.
 * A mayor número de prioridad, mayor es la prioridad del mensaje. Por ejemplo, el mensaje con prioridad 3 se recibe
 * antes que el mensaje con prioridad 1. * 
 * 
 * b) ¿Que sucede si se cambia O_RDWR por O_RDONLY? ¿Y si se cambia por O_WRONLY?
 * 
 * Si se cambia O_RDWR por O_RDONLY, el programa no puede enviar mensajes. Si se cambia por O_WRONLY, el programa no
 * puede recibir mensajes.
 * 
 */

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE 1024
#define MQ_NAME "/mq_example"
#define N_MESSAGES 6

int main(void) {
  char aux[MAX_MESSAGE];
  int i;
  struct mq_attr attributes;
  mqd_t mq;
  unsigned int priors[N_MESSAGES] = {1, 1, 1, 2, 1, 3}, prior;

  attributes.mq_maxmsg = 10;
  attributes.mq_msgsize = MAX_MESSAGE;

  if ((mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes)) == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }
  mq_unlink(MQ_NAME);

  for (i = 0; i < N_MESSAGES; i++) {
    sprintf(aux, "Message %d", i + 1);
    printf("Sending:  %s\n", aux);
    if (mq_send(mq, aux, strlen(aux) + 1, priors[i]) == -1) {
      perror("mq_send");
      mq_close(mq);
      exit(EXIT_FAILURE);
    }
  }
  printf("\n");
  for (i = 0; i < N_MESSAGES; i++) {
    if (mq_receive(mq, aux, MAX_MESSAGE, &prior) == -1) {
      perror("mq_receive");
      mq_close(mq);
      exit(EXIT_FAILURE);
    }
    printf("Received: %s (Priority: %u)\n", aux, prior);
  }

  mq_close(mq);

  exit(EXIT_SUCCESS);
}
