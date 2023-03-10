EXE = voting
FLAGS = -g -Wall -ansi -pedantic
LIBS = -lpthread -lm -lbsd

all: $(EXE)

pow.o: pow.c
	gcc $(FLAGS) -c pow.c

monitor.o: monitor.c 
	gcc $(FLAGS) -c monitor.c

monitor: monitor.o
	gcc $(FLAGS) -o monitor.o

minero.o: minero.c
	gcc $(FLAGS) -c minero.c

voting.o: voting.c
	gcc $(FLAGS) -c voting.c

voting: voting.o minero.o pow.o monitor.o
	gcc $(FLAGS) -o voting voting.o minero.o pow.o monitor.o $(LIBS)

clean:
	rm -f *.o $(EXE) pids.txt