cc = gcc -Wall

all: clean compile test

compile:
	$(cc) macierz.c -o macierz
	$(cc) helper.c -o helper

clean:
	rm -f macierz results.txt helper
	find ./tests -name 'matrix*' -exec rm {} \;
	find ./tests -name 'result*' -exec rm {} \;
	rm -f ./tests/source.txt

separate:
	./macierz source.txt 1 2 separate

common:
	./macierz source.txt 1 2 common

test:
	./helper 2 2 5
	./helper 6 5 10
	./helper 10 10 15

setup:
	mkdir tests