/**
 * @file mq_receptor.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) Ejecutar el c ́odigo del emisor, y despu ́es el del receptor. ¿Qu ́e sucede? ¿Por qu ́e?
 * 
 * El receptor recibe el mensaje del emisor. Esto se debe a que el receptor se ejecuta después del emisor.
 * 
 * b) Ejecutar el c ́odigo del receptor, y despu ́es el del emisor. ¿Qu ́e sucede? ¿Por qu ́e?
 * 
 * El receptor se queda esperando a que el emisor envíe un mensaje. Esto se debe a que el receptor se ejecuta antes del emisor.
 * La función mq_receive es bloqueante, por lo que el receptor se queda esperando a que el emisor envíe un mensaje.
 * 
 * c) Repetir las pruebas anteriores creando la cola de mensajes como no bloqueante.
 * ¿Qu ́e sucede ahora?
 * 
 * Ahora, el receptor no se queda esperando a que el emisor envíe un mensaje. Esto se debe a que la cola de mensajes se crea como no bloqueante.
 * Pero como no hay mensajes en la cola, el receptor no recibe nada.
 * 
 * d) Si hubiera mas de un receptor en el sistema, ¿serıa adecuado sincronizar los accesos
 * a la cola usando sem ́aforos? ¿Por que?
 * 
 * No sería adecuado sincronizar los accesos a la cola usando semáforos. Esto se debe a que la cola de mensajes ya tiene mecanismos de sincronización.
 * Los mecanismos de sincronización de la cola de mensajes son:
 * - mq_send: bloqueante. Si la cola está llena, el proceso se bloquea hasta que haya espacio en la cola.
 * - mq_receive: bloqueante. Si la cola está vacía, el proceso se bloquea hasta que haya mensajes en la cola.
 * - mq_open: no bloqueante. Si la cola no existe, se crea.
 * 
 * De esta forma, la cola de mensajes ya tiene mecanismos de sincronización, por lo que no es necesario usar semáforos.
 * 
 */

#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "mq_sd_rc.h"

int main(void) {
  Message msg;
  mqd_t queue;

  queue = mq_open(
      MQ_NAME,
      O_CREAT | O_RDONLY | O_NONBLOCK, /* This process is going to receive messages. */
      S_IRUSR | S_IWUSR,  /* The user can read and write. */
      &attributes);

  if (queue == (mqd_t)-1) {
    fprintf(stderr, "Error opening the queue\n");
    return EXIT_FAILURE;
  }

  if (mq_receive(queue, (char *)&msg, sizeof(msg), NULL) == -1) {
    fprintf(stderr, "Error receiving message\n");
    return EXIT_FAILURE;
  }

  printf("%d: %s\n", msg.value, msg.warning);

  /* Wait for input to end the program. */
  fprintf(stdout, "Press any key to finish\n");
  getchar();

  mq_close(queue);

  return EXIT_SUCCESS;
}
