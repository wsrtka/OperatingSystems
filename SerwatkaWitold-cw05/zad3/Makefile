cc = gcc -Wall

all: clean producer consumer main

test: clean producer consumer main
	./main

producer:
	$(cc) producer.c -o producer

consumer:
	$(cc) consumer.c -o consumer

main:
	$(cc) main.c -o main

setup:
	head -n 4 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcFile.txt
	head -n 2 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcFile1.txt
	head -n 2 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcFile2.txt
	head -n 2 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcFile3.txt
	head -n 2 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcFile4.txt
	head -n 2 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcFile5.txt
	mkfifo pajposz

clean:
	rm -f main producer consumer destFile.txt srcFile.txt srcFile1.txt srcFile2.txt srcFile3.txt srcFile4.txt srcFile5.txt pajposz