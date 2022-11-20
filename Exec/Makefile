.PHONY:all
all:myexec mybin mycpp

myexec:myexec.c
	gcc -o $@ $^
mybin:mybin.c
	gcc -o $@ $^
mycpp:mycpp.cpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f myexec mybin mycpp
