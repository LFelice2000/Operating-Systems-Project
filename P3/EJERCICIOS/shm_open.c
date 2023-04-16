/**
 * @brief Ejemplo de uso de shm_open
 *
 * a) ¿En que consiste este codigo? ¿Que sentido tiene utilizarlo para abrir un objeto de
 * memoria compartida?
 * 
 * El código consiste en abrir un objeto de memoria compartida, si no existe se crea, si existe
 * se abre. El sentido de utilizarlo es para abrir un objeto de memoria compartida y evitar 
 * errores de apertura. Sólo un proceso puede crear un objeto de memoria compartida, si otro
 * proceso intenta crearlo, se producirá un error. Si el objeto de memoria compartida ya existe
 * se abre y se puede utilizar. 
 *
 *
 * b) En un momento dado se deseara forzar (en la pr ́oxima ejecucion del programa) la
 * inicializacion del objeto de memoria compartida SHM_NAME. ¿Que posibles soluciones,
 * en codigo C o fuera de el, hay para forzar dicha inicializacion?
 * 
 * Una posible solución es eliminar el objeto de memoria compartida con el comando:
 * rm /dev/shm/SHM_NAME y volver a ejecutar el programa.
 *
 */

/* This is only a code fragment, not a complete program... */
fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
if (fd_shm == -1)
{
	if (errno == EEXIST)
	{
		fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
		if (fd_shm == -1)
		{
			perror("Error opening the shared memory segment");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Shared memory segment open\n");
		}
	}
	else
	{
		perror("Error creating the shared memory segment\n");
		exit(EXIT_FAILURE);
	}
}
else
{
	printf("Shared memory segment created\n");
}
