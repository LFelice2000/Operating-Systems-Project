EXE = minero
FLAGS = -Wall -pedantic -ansi

all: $(EXE)

pow.o: pow.c
	gcc $(FLAGS) -c pow.c

minero.o: minero.c
	gcc $(FLAGS) -c minero.c

minero: minero.o pow.o
	gcc $(FLAGS) -o minero minero.o pow.o

clean:
	rm -f *.o $(EXE)