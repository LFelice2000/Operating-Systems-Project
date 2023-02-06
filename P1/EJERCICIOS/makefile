EXE = ej3 ej4 ej5 ej6 ej7 ej8 ej10 ej11 ej12

all: $(EXE)

ej3.o: ej3.c
	gcc -c ej3.c

ej3: ej3.o
	gcc -o ej3 ej3.o

ej4.o: ej4.c
	gcc -c ej4.c

ej4: ej4.o
	gcc -o ej4 ej4.o

thread_example.o: thread_example.c
	gcc -c thread_example.c

ej5: thread_example.o
	gcc -o ej5 thread_example.o -lpthread

proc_example.o: proc_example.c
	gcc -c proc_example.c

ej6: proc_example.o
	gcc -o ej6 proc_example.o

proc_malloc.o: proc_malloc.c
	gcc -c proc_malloc.c

ej7: proc_malloc.o
	gcc -o ej7 proc_malloc.o

proc_exec.o: proc_exec.c
	gcc -c proc_exec.c

ej8: proc_exec.o
	gcc -o ej8 proc_exec.o

file_descriptors.o: file_descriptors.c
	gcc -c file_descriptors.c

ej10: file_descriptors.o
	gcc -o ej10 file_descriptors.o

file_buffer.o: file_buffer.c
	gcc -c file_buffer.c

ej11: file_buffer.o
	gcc -o ej11 file_buffer.o

pipe_example.o: pipe_example.c
	gcc -c pipe_example.c

ej12: pipe_example.o
	gcc -o ej12 pipe_example.o

clean:
	rm -f *.o $(EXE)