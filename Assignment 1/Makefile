CC=gcc --std=gnu99 -g
exe_file = movies

all: link

link: linkedlist.o methods.o movies.o
	$(CC) linkedlist.o methods.o movies.o -o $(exe_file)

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) -c linkedlist.c

movies.o: movies.c movies.h
	$(CC) -c movies.c

methods.o: methods.c methods.h
	$(CC) -c methods.c

clean:
	rm -f *.out *.o $(exe_file)