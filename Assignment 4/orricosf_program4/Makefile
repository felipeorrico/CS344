CC=gcc --std=gnu99 -g -lm -pthread
DD=gcc --std=gnu99 -g -lm -lpthread
exe_file = line_processor

all: link

other: linkedlistl.o mainl.o
	$(DD) linkedlist.o main.o -o $(exe_file)

link: linkedlist.o main.o
	$(CC) linkedlist.o main.o -o $(exe_file)

main.o: main.c
	$(CC) -c main.c

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) -c linkedlist.c

mainl.o: main.c
	$(DD) -c main.c

linkedlistl.o: linkedlist.c linkedlist.h
	$(DD) -c linkedlist.c

clean:
	rm -f *.out *.o $(exe_file)