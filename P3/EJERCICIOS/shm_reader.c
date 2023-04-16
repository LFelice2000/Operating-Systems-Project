/**
 * @file shm_reader.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-16
 * 
 * @copyright Copyright (c) 2023
 * 
 * a) ¿Tendr ́ıa sentido incluir shm_unlink en el lector? ¿Por qu ́e?
 * 
 * No tendría sentido incluir shm_unlink en el lector, ya que el lector no crea el segmento de memoria compartida, sino que lo abre y lo lee.
 * 
 * b) ¿Tendrıa sentido incluir ftruncate en el lector? ¿Por que?
 * 
 * No tendría sentido incluir ftruncate en el lector, ya que el lector no crea el segmento de memoria compartida, sino que lo abre y lo lee.
 * 
 * c) ¿Cual es la diferencia entre shm_open y mmap? ¿Qu ́e sentido tiene que existan dos
 * funciones diferentes?
 * 
 * La diferencia entre shm_open y mmap es que shm_open crea un segmento de memoria compartida y mmap lo mapea en memoria. Tienen sentido dos funciones diferentes porque
 * es necesario distinguir entre la creación del segmento de memoria compartida y el mapeo de la memoria compartida en memoria.
 * 
 * d) ¿Se podrıa haber usado la memoria compartida sin enlazarla con mmap? Si es ası,
 * ¿como se harıa?
 * 
 * No se podría haber usado la memoria compartida sin enlazarla con mmap, ya que mmap es la única forma de mapear la memoria compartida en memoria. Si no se mapea la memoria
 * compartida en memoria, no se puede acceder a ella. Aun así, se podría haber usado la memoria compartida sin enlazarla con mmap, pero habría que usar funciones como
 * shmget, shmat, shmdt, etc.
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "shm_rdwr.h"

int main(void) {
  int i, fd_shm;
  ShmExampleStruct *shm_struct = NULL;

  /* Open of the shared memory. */
  if ((fd_shm = shm_open(SHM_NAME, O_RDONLY, 0)) == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  /* Mapping of the memory segment. */
  shm_struct = mmap(NULL, sizeof(ShmExampleStruct), PROT_READ, MAP_SHARED, fd_shm, 0);
  close(fd_shm);
  if (shm_struct == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  printf("Pointer to shared memory segment: %p\n", (void *)shm_struct);

  /* Reading of the memory. */
  printf("%s\n", shm_struct->message);
  for (i = 0; i < INT_LIST_SIZE; i++) {
    printf("%d\n", shm_struct->integer_list[i]);
  }

  /* Unmapping of the shared memory */
  munmap(shm_struct, sizeof(ShmExampleStruct));

  exit(EXIT_SUCCESS);
}
