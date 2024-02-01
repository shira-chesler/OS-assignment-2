
CC = gcc
CFLAGS = -Wall -pthread
LIBS = -lssl -lcrypto

.PHONY: clean all

all: server basicClient


server: server.c utils.c
	$(CC) $(CFLAGS) -o server server.c utils.c $(LIBS)

basicClient: basicClient.c utils.c
	$(CC) $(CFLAGS) -o basicClient basicClient.c utils.c $(LIBS)



clean:
	rm -f server basicClient
