CC = gcc -Wall

clean:
	rm find find_nftw

main:
	make clean
	$(CC) main.c -o main

tests:
	make clean
	make main
	./main ./test -mtime 2 -atime 3
	./main ./test -mtime 1 -atime 1 -maxdepth 1