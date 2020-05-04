cc = gcc -Wall -lpthread

main: clean
	$(cc) -o main main.c

test:
	./main 1 sign mona_lisa.ascii.pgm wynik.txt


clean:
	rm -f main