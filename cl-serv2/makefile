server: server.o
	g++ server.o -o server

client: client.o
	g++ client.o -o client

client2: client2.o
	g++ client2.o -o client2	

server.o: server.cpp
	g++ -c server.cpp

client.o: client.cpp
	g++ -c client.cpp -o client.o

client2.o: client.cpp
	g++ -c client.cpp -o client2.o

clean:
	rm -f server client client2 server.o client.o client2.o