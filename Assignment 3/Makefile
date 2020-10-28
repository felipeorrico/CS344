CC=gcc --std=gnu99 -g
exe_file = smallsh

all: link

link: linkedlist.o intList.o utils.o Shell.o cd.o handler.o exit.o main.o
	$(CC) linkedlist.o intList.o utils.o Shell.o cd.o handler.o exit.o main.o -o $(exe_file)

main.o: main.c
	$(CC) -c main.c

linkedlist.o: Utils/linkedlist.c Utils/linkedlist.h
	$(CC) -c Utils/linkedlist.c

intList.o: Utils/intList.c Utils/intList.h
	$(CC) -c Utils/intList.c

utils.o: Utils/utils.c Utils/utils.h
	$(CC) -c Utils/utils.c

Shell.o: Shell/Shell.c Shell/Shell.h
	$(CC) -c Shell/Shell.c

cd.o: Built-in/cd.c Built-in/cd.o
	$(CC) -c Built-in/cd.c

exit.o: Built-in/exit.c Built-in/exit.h
	$(CC) -c Built-in/exit.c

handler.o: Utils/handler.c Utils/handler.h
	$(CC) -c Utils/handler.c

clean:
	rm -f *.out *.o $(exe_file)