EXE = voting
FLAGS = -g -pedantic
LIBS = -lpthread

all: $(EXE)

pow.o: pow.c
	gcc $(FLAGS) -c pow.c

monitor.o: monitor.c 
	gcc $(FLAGS) -c monitor.c

monitor: monitor.o
	gcc $(FLAGS) -o monitor.o

minero.o: minero.c
	gcc $(FLAGS) -c minero.c

votante.o: votante.c
	gcc $(FLAGS) -c votante.c

voting.o: voting.c
	gcc $(FLAGS) -c voting.c

voting: voting.o votante.o
	gcc $(FLAGS) -o voting voting.o votante.o $(LIBS)

clean:
	rm -f *.o $(EXE) *.txt