cc = gcc -Wall

all: clean catcher sender

catcher: clean
	$(cc) catcher.c -o catcher

sender: clean	
	$(cc) sender.c -o sender

clean:
	rm -f sender catcher