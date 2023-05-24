.PHONY:all
all:calServer calClient

calServer:calServer.cc
	g++ -o $@ $^ -std=c++11 -l jsoncpp #-DMYSELF

calClient:calClient.cc
	g++ -o $@ $^ -std=c++11 -l jsoncpp #-DMYSELF

.PHONY:clean
clean:
	rm -f calServer calClient