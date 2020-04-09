cc = gcc -Wall

all: clean producer consumer main

test: clean producer consumer

producer:
	$(cc) producer.c -o producer

consumer:
	$(cc) consumer.c -o consumer

main:
	$(cc) main.c -o main

clean:
	rm -f main producer consumer