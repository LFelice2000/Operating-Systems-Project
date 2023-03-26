EXE = ej2 ej3 ej4 ej5 ej6 ej7 ej8 ej9

all: $(EXE)

sig_kill.o: sig_kill.c
	gcc -c sig_kill.c

ej2: sig_kill.o
	gcc -o ej2 sig_kill.o

sig_capture.o: sig_capture.c
	gcc -c sig_capture.c

ej3: sig_capture.o
	gcc -o ej3 sig_capture.o

sig_capture_improved.o: sig_capture_improved.c
	gcc -c sig_capture_improved.c

ej4:  sig_capture_improved.o
	gcc -o ej4  sig_capture_improved.o

sig_sigset.o: sig_sigset.c
	gcc -c sig_sigset.c

ej5:  sig_sigset.o
	gcc -o ej5  sig_sigset.o

sig_alarm.o: sig_alarm.c
	gcc -c sig_alarm.c

ej6:  sig_alarm.o
	gcc -o ej6  sig_alarm.o

sem_create.o: sem_create.c
	gcc -c sem_create.c

ej7:  sem_create.o
	gcc -o ej7  sem_create.o

sem_signal.o: sem_signal.c
	gcc -c sem_signal.c

ej8:  sem_signal.o
	gcc -o ej8  sem_signal.o

conc_alternate.o: conc_alternate.c
	gcc -c conc_alternate.c

ej9:  conc_alternate.o
	gcc -o ej9  conc_alternate.o

clean:
	rm -f *.o $(EXE)
