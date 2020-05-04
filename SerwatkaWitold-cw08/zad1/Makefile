cc = gcc -Wall
libs = -lpthread

main: clean
	$(cc) -o main main.c $(libs)

test:
	./main 1 sign mona_lisa.ascii.pgm wynik.txt


clean:
	rm -f main