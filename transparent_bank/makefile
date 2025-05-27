all:
	g++ -std=c++17 -pthread initbank.cpp -o initbank -lrt
	g++ -std=c++17 -pthread deinitbank.cpp -o deinitbank -lrt
	g++ -std=c++17 -pthread server.cpp -o server -lrt
	g++ -std=c++17 -pthread client.cpp -o client

clean:
	rm -f initbank deinitbank server client
