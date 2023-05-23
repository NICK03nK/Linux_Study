.PHONY:all
all:calServer calClient

calServer:calServer.cc
	g++ -o $@ $^ -std=c++11

calClient:calClient.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f calServer calClient