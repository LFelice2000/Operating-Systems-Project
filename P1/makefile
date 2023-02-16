EXE = mrush
FLAGS = -Wall -pedantic
LIBS = -lpthread -lm

all: $(EXE)

pow.o: pow.c
	gcc $(FLAGS) -c pow.c

monitor.o: monitor.c 
	gcc $(FLAGS) -c monitor.c

monitor: monitor.o
	gcc $(FLAGS) -o monitor.o

minero.o: minero.c
	gcc $(FLAGS) -c minero.c

mrush.o: mrush.c
	gcc $(FLAGS) -c mrush.c

mrush: mrush.o minero.o pow.o monitor.o
	gcc $(FLAGS) -o mrush mrush.o minero.o pow.o monitor.o $(LIBS)

clean:
	rm -f *.o $(EXE)