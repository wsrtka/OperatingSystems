cc = gcc -Wall

all: clean producer consumer main

test: clean producer consumer

producer:
	$(cc) producer.c -o producer

consumer:
	$(cc) consumer.c -o consumer

main:
	$(cc) main.c -o main

setup:
	head -n 4 /dev/urandom | tr -dc A-Za-z0-9 | fold -w 10 > srcfile.txt
	mkfifo pajposz

clean:
	rm -f main producer consumer dest_file.txt src_file.txt