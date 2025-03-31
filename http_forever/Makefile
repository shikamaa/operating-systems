httpforever: httpforever.o
	g++ httpforever.o -o httpforever

httpforever.o: http_forever.cpp
	g++ -c http_forever.cpp -o httpforever.o

clean:
	rm -f httpforever httpforever.o
