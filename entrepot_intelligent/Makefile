CC = gcc
CFLAGS = -Wall -pthread

all: server client

server: server/gestionnaire_outils.c
	$(CC) $(CFLAGS) -o server/server server/gestionnaire_outils.c

client: client/bras_robotique.c
	$(CC) $(CFLAGS) -o client/client client/bras_robotique.c

clean:
	rm -f server/server client/client

run-server:
	./server/server

run-client:
	./client/client 1 0 1

.PHONY: all clean run-server run-client
