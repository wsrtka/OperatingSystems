cc = gcc -Wall
libs = -lpthread

main: clean
	$(cc) -o main main.c $(libs)

test:
	./main 1 sign mona_lisa.ascii.pgm wynik.txt
	./main 2 sign mona_lisa.ascii.pgm wynik.txt
	./main 4 sign mona_lisa.ascii.pgm wynik.txt
	./main 8 sign mona_lisa.ascii.pgm wynik.txt

	./main 1 block mona_lisa.ascii.pgm wynik.txt
	./main 2 block mona_lisa.ascii.pgm wynik.txt
	./main 4 block mona_lisa.ascii.pgm wynik.txt
	./main 8 block mona_lisa.ascii.pgm wynik.txt

	./main 1 interleaved mona_lisa.ascii.pgm wynik.txt
	./main 2 interleaved mona_lisa.ascii.pgm wynik.txt
	./main 4 interleaved mona_lisa.ascii.pgm wynik.txt
	./main 8 interleaved mona_lisa.ascii.pgm wynik.txt

clean:
	rm -f main wynik.txt