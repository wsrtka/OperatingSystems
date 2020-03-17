CC = gcc -Wall

clean:
	rm find find_nftw

find:
	make clean
	$(CC) find.c -o find

nftw:
	make clean
	$(CC) find_nftw.c -o find

test:
	make find
	./find ./test -mtime 2 -atime 3
	./find ./test -mtime 1 -atime 1 -maxdepth 1
	make nftw
	./find ./test -mtime 2 -atime 3
	./find ./test -mtime 1 -atime 1 -maxdepth 1