EXE = file_truncate file_mmap shm_writer shm_reader mq_send_receive mq_sender mq_receptor

all: $(EXE)

file_truncate: file_truncate.o
	gcc -o file_truncate file_truncate.o

file_truncate.o: file_truncate.c
	gcc -c file_truncate.c

file_mmap: file_mmap.o
	gcc -o file_mmap file_mmap.o

file_mmap.o: file_mmap.c
	gcc -c file_mmap.c

shm_writer: shm_writer.o
	gcc -o shm_writer shm_writer.o

shm_writer.o: shm_writer.c
	gcc -c shm_writer.c

shm_reader: shm_reader.o
	gcc -o shm_reader shm_reader.o

shm_reader.o: shm_reader.c
	gcc -c shm_reader.c

mq_send_receive: mq_send_receive.o
	gcc -o mq_send_receive mq_send_receive.o

mq_send_receive.o: mq_send_receive.c
	gcc -c mq_send_receive.c

mq_sender: mq_sender.o
	gcc -o mq_sender mq_sender.o

mq_sender.o: mq_sender.c
	gcc -c mq_sender.c

mq_receptor: mq_receptor.o
	gcc -o mq_receptor mq_receptor.o

clean:
	rm -f *.o $(EXE)