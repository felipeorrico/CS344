CC=gcc --std=gnu99 -g

all: compile

compile: keygen enc_client enc_server dec_client dec_server

keygen: keygen.c
	$(CC) keygen.c -o keygen

enc_server: intlist.c intlist.h ServerAPI.h ServerAPI.c enc_server.c
	$(CC) intlist.c ServerAPI.c enc_server.c -o enc_server

dec_server: intlist.c intlist.h ServerAPI.h ServerAPI.c dec_server.c
	$(CC) intlist.c ServerAPI.c dec_server.c -o dec_server

enc_client: enc_client.c
	$(CC) enc_client.c -o enc_client

dec_client: dec_client.c
	$(CC) dec_client.c -o dec_client

clean:
	rm -f *.out *.o keygen enc_server dec_server enc_client dec_client