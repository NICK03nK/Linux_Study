.PHONY:all
all:tcpServer tcpClient

tcpServer:tcpServer.cc
	g++ -o $@ $^ -std=c++11 -l pthread

tcpClient:tcpClient.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f tcpServer tcpClient