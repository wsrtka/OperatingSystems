cc = gcc -Wall
libs = -lpthread

all: clean server client

debug:
	$(cc) -g -o server server.c $(libs)
	$(cc) -g -o client client.c $(libs)

test:
	rm -f test
	./server 3333 test

server:
	$(cc) -o server server.c $(libs)

client:
	$(cc) -o client client.c $(libs)

clean:
	rm -f server client test