cc = gcc -Wall

all: clean main receiver loader sender

main:
	$(cc) -o main main.c

receiver:
	$(cc) -o receiver receiver.c

loader:
	$(cc) -o loader loader.c

sender:
	$(cc) -o sender sender.c

clean:
	rm -f main receiver loader sender