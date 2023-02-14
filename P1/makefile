EXE = minero mrush monitor
FLAGS = -Wall -pedantic

all: $(EXE)

pow.o: pow.c
	gcc $(FLAGS) -c pow.c

minero.o: minero.c
	gcc $(FLAGS) -c minero.c 

minero: minero.o pow.o
	gcc $(FLAGS) -pthread -o minero minero.o pow.o -lm

mrush.o: mrush.c
	gcc $(FLAGS) -c mrush.c

mrush: mrush.o
	gcc $(FLAGS) -o mrush mrush.o

monitor.o: monitor.c 
	gcc $(FLAGS) -c monitor.c

monitor: monitor.o
	gcc $(FLAGS) -o monitor monitor.o

clean:
	rm -f *.o $(EXE)