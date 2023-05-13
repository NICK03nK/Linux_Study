.PHONY:all
all:tcpServer tcpClient

tcpServer:tcpServer.cc
	g++ -o $@ $^ -std=c++11

tcpClient:tcpClient.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f tcpServer tcpClient