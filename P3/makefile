EXE = miner monitor
FLAGS = -g -pedantic -Wall -pedantic
LIBS = -lpthread -lrt

all: $(EXE)

pow.o: pow.c pow.h
	gcc $(FLAGS) -c pow.c

miner: miner.o pow.o
	gcc $(FLAGS) -o miner miner.o pow.o $(LIBS)

miner.o: miner.c
	gcc $(FLAGS) -c miner.c

monitor: monitor.o pow.o
	gcc $(FLAGS) -o monitor monitor.o pow.o $(LIBS)

clean:
	rm -f *.o $(EXE)