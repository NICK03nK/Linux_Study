libmymath.a:add.o sub.o
	ar -rc $@ $^

add.o:add.c
	gcc -c add.c -o add.o

sub.o:sub.c
	gcc -c sub.c -o sub.o

.PHONY:output
output:
	mkdir -p mylib/include
	mkdir -p mylib/lib
	cp -f *.a mylib/lib
	cp -f *.h mylib/include

.PHONY:clean
clean:
	rm -rf *.o libmymath.a mylib
