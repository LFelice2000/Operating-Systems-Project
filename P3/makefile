EXE = miner monitor
FLAGS = -g -pedantic -Wall -pedantic
LIBS = -lpthread

all: $(EXE)

miner: miner.o
	gcc $(FLAGS) -o miner miner.o $(LIBS)

miner.o: miner.c
	gcc $(FLAGS) -c miner.c

monitor: monitor.o
	gcc $(FLAGS) -o monitor monitor.o $(LIBS)

clean:
	rm -f *.o $(EXE)