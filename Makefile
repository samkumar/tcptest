all: tcp_server tcp_client

tcp_server: utils.o tcpstr.o tcp_server.o
	gcc -pthread utils.o tcpstr.o tcp_server.o -o tcp_server
	
tcp_client: utils.o tcpstr.o tcp_client.o
	gcc -pthread utils.o tcpstr.o tcp_client.o -o tcp_client

utils.o:
	gcc -pthread -c utils.c -o utils.o

tcpstr.o:
	gcc -pthread -c tcpstr.c -o tcpstr.o
	
tcp_server.o:
	gcc -pthread -c tcp_server.c -o tcp_server.o
	
tcp_client.o:
	gcc -pthread -c tcp_client.c -o tcp_client.o
	
clean:
	rm -f tcp_server tcp_client *.o *~
