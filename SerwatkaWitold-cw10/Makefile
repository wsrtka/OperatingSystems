cc = gcc -Wall
libs = -lpthread

all: clean server client

server:
	$(cc) -o server server.c $(libs)

client:
	$(cc) -o client client.c $(libs)

clean:
	rm -f server client