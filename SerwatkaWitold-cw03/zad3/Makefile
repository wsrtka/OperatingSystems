clean:
	rm -f macierz results.txt helper
	find ./tests -name 'matrix*' -exec rm {} \;
	rm -f ./tests/source.txt

compile:
	gcc -Wall macierz.c -o macierz
	gcc -Wall helper.c -o helper

separate:
	./macierz source.txt 1 2 separate

common:
	./macierz source.txt 1 2 common

all:
	make clean
	make compile
	make test

gen:
	gcc -Wall helper.c -o helper

test:
	./helper 2 2 5

debug:
	make compile
	make test

setup:
	mkdir tests