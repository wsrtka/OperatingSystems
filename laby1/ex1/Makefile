clean:
	rm -f *.o main

static:
	make clean
	gcc $(CFLAGS) -c my_library.c
	ar rcs mylib.a my_library.o
	gcc $(CFLAGS) -o main main.c -L. -lmy_library

shared:
	make clean
	gcc $(CFLAGS) -fPIC -c my_library.c
	gcc -shared -o mylib.so my_library.o
	gcc $(CFLAGS) -o main main.c -L. -lmy_library

dynamic:
	make clean
	gcc $(CFLAGS) -fPIC -c my_library.c
	gcc -shared -o mylib.so my_library.o
	gcc $(CFLAGS) -o main_dynamic main_dynamic.c