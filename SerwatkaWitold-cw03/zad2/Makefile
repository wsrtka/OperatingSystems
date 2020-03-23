clean:
	rm macierz results.txt

compile:
	gcc -Wall macierz.c -o macierz

separate:
	./macierz source.txt 1 2 separate

common:
	./macierz source.txt 1 2 common

all:
	make clean
	make compile
	make test