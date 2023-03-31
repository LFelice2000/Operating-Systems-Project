EXE = voting
FLAGS = -g -pedantic
LIBS = -lpthread

all: $(EXE)

votante.o: votante.c
	gcc $(FLAGS) -c votante.c

voting.o: voting.c
	gcc $(FLAGS) -c voting.c

voting: voting.o votante.o
	gcc $(FLAGS) -o voting voting.o votante.o $(LIBS)

clean:
	rm -f *.o $(EXE) *.txt