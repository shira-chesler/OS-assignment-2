
CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto


all: server basicClient

.PHONY: clean all


server: server.o
	$(CC) $(CFLAGS) -o server server.o $(LIBS)

basicClient: basicClient.o
	$(CC) $(CFLAGS) -o basicClient basicClient.o $(LIBS)



clean:
	rm -f server basicClient *.o
