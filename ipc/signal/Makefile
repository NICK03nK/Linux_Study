.PHONY:all
all:mySignal myTest

myTest:myTest.cpp
	g++ -o $@ $^ -std=c++11

mySignal:mySignal.cpp
	g++ -o $@ $^ -std=c++11 -g

.PHONY:clean
clean:
	rm -rf mySignal myTest