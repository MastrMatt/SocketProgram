CC = gcc
CFLAGS = -Wall -std=gnu11 -g

all: client server mainDriver

client: mainHeader.h client.c
	$(CC) $(CFLAGS) -o client client.c

server: mainHeader.h server.c jobFunctions.c
	$(CC) $(CFLAGS) -o server server.c jobFunctions.c

testDriver: mainHeader.h mainDriver.c client.c server.c
	$(CC) $(CFLAGS) -o mainDriver mainDriver.c


