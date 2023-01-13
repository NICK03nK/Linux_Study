libmymath.so:add.o sub.o
	gcc -shared -o $@ $^

add.o:add.c
	gcc -c -fPIC add.c

sub.o:sub.c
	gcc -c -fPIC sub.c

.PHONY:output
output:
	mkdir -p mylib/include
	mkdir -p mylib/lib
	cp -f *.so mylib/lib
	cp -f *.h mylib/include

.PHONY:clean
clean:
	rm -rf *.o libmymath.so mylib
