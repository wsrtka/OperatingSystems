cc = gcc -Wall

all: clean
	$(cc) main.c -o main
	$(cc) exec.c -o exec

clean:
	rm -f main exec

test:
	./main fork ignore
	./main fork handler
	./main fork mask
	./main fork pending
	./main exec ignore
	./main exec handler
	./main exec pending