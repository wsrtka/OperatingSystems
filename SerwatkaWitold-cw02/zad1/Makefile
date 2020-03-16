clean:
	rm -f *.o main *.a

static:
	make clean
	gcc $(CFLAGS) -c myfslib.c
	ar rcs myfslib.a myfslib.o
	gcc $(CFLAGS) -o main main.c myfslib.a

test:
	make clean
	make static

	# rekord: 1B
	./main generate file 10000 1 copy file file1 10000 1 sys copy file file2 10000 1 lib sort file1 10000 1 sys sort file2 10000 1 lib
	./main generate file 20000 1 copy file file1 20000 1 sys copy file file2 20000 1 lib sort file1 20000 1 sys sort file2 20000 1 lib
	./main generate file 40000 1 copy file file1 40000 1 sys copy file file2 40000 1 lib sort file1 40000 1 sys sort file2 40000 1 lib

	# rekord: 4B
	./main generate file 2500 4 copy file file4 2500 4 sys copy file file2 2500 4 lib sort file4 2500 4 sys sort file2 2500 4 lib
	./main generate file 5000 4 copy file file4 5000 4 sys copy file file2 5000 4 lib sort file4 5000 4 sys sort file2 5000 4 lib
	./main generate file 40000 4 copy file file4 40000 4 sys copy file file2 40000 4 lib sort file4 40000 4 sys sort file2 40000 4 lib

	# rekord: 512B
	./main generate file 2000 512 copy file file1 2000 512 sys copy file file2 2000 512 lib sort file1 2000 512 sys sort file2 2000 512 lib
	./main generate file 4000 512 copy file file1 4000 512 sys copy file file2 4000 512 lib sort file1 4000 512 sys sort file2 4000 512 lib
	./main generate file 8000 512 copy file file1 8000 512 sys copy file file2 8000 512 lib sort file1 8000 512 sys sort file2 8000 512 lib

	# rekord: 1024B
	./main generate file 2000 1024 copy file file1 2000 1024 sys copy file file2 2000 1024 lib sort file1 2000 1024 sys sort file2 2000 1024 lib
	./main generate file 4000 1024 copy file file1 4000 1024 sys copy file file2 4000 1024 lib sort file1 4000 1024 sys sort file2 4000 1024 lib
	./main generate file 8000 1024 copy file file1 8000 1024 sys copy file file2 8000 1024 lib sort file1 8000 1024 sys sort file2 8000 1024 lib

	# rekord: 4096B
	./main generate file 1000 4096 copy file file1 1000 4096 sys copy file file2 1000 4096 lib sort file1 1000 4096 sys sort file2 1000 4096 lib
	./main generate file 2000 4096 copy file file1 2000 4096 sys copy file file2 2000 4096 lib sort file1 2000 4096 sys sort file2 2000 4096 lib
	./main generate file 4000 4096 copy file file1 4000 4096 sys copy file file2 4000 4096 lib sort file1 4000 4096 sys sort file2 4000 4096 lib

	# rekord: 8192B
	./main generate file 1000 8192 copy file file1 1000 8192 sys copy file file2 1000 8192 lib sort file1 1000 8192 sys sort file2 1000 8192 lib
	./main generate file 2000 8192 copy file file1 2000 8192 sys copy file file2 2000 8192 lib sort file1 2000 8192 sys sort file2 2000 8192 lib
	./main generate file 4000 8192 copy file file1 4000 8192 sys copy file file2 4000 8192 lib sort file1 4000 8192 sys sort file2 4000 8192 lib