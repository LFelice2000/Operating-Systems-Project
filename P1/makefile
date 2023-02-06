EXE = minero

all: $(EXE)

pow.o: pow.c
	gcc -c pow.c

minero.o: minero.c
	gcc -c minero.c

minero: minero.o
	gcc -o minero minero.o pow.o

clean:
	rm -f *.o $(EXE)
