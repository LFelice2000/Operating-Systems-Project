EXE = miner monitor
FLAGS = -g -ansi -pedantic -Wall
LIBS = -lpthread -lrt -lm

all: $(EXE)

pow.o: pow.c pow.h
	gcc $(FLAGS) -c pow.c

bloque.o: bloque.c bloque.h
	gcc $(FLAGS) -c bloque.c

sistema.o: sistema.c sistema.h
	gcc $(FLAGS) -c sistema.c

miner_func.o: miner_func.c miner_func.h
	gcc $(FLAGS) -c miner_func.c

miner.o: miner.c
	gcc $(FLAGS) -c miner.c

miner: miner.o pow.o bloque.o sistema.o miner_func.o
	gcc $(FLAGS) -o miner miner.o pow.o bloque.o sistema.o miner_func.o $(LIBS)

monitor: monitor.o pow.o
	gcc $(FLAGS) -o monitor monitor.o pow.o $(LIBS)

clean:
	rm -f *.o $(EXE) *.txt
	rm -f /dev/shm/sem.sem_minero
	rm -f /dev/shm/sem.sem_mqueue
	rm -f /dev/shm/sem.sem_winner
	rm -f /dev/shm/shm_monitor
	rm -f /dev/shm/shm_sistema