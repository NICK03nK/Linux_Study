.PHONY:all
all:udpClient udpServer

udpClient:udpClient.cc
	g++ -o $@ $^ -std=c++11 -l pthread

udpServer:udpServer.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f udpClient udpServer