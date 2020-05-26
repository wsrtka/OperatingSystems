cc = gcc -Wall

all: clean server client

server:
	$(cc) server.c -o server -lrt

client:
	$(cc) client.c -o client -lrt

clean:
	rm -f client server
