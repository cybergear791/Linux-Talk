both: server.x client.x

server.x:  server.o 
	g++ -ggdb -o server.x server.o -lcurses -ltermcap

server.o: server.cpp
	g++ -std=c++11 -c -ggdb server.cpp -o server.o

client.x: client.o 
	g++ -ggdb -o client.x client.o -lcurses -ltermcap

client.o: client.cpp
	g++ -std=c++11  -c -ggdb client.cpp -o client.o

clean:
	/bin/rm -f server.x server.o client.x client.o *~
