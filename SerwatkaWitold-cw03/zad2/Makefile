clean:
	rm macierz

compile:
	gcc -Wall macierz.c -o macierz

test:
	./macierz source.txt 1 2 separate

all:
	make clean
	make compile
	make test