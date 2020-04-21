cc = gcc -Wall

all: clean server client

server:
	$(cc) server.c -o server

client:
	$(cc) client.c -o client

clean:
	rm -f client server
