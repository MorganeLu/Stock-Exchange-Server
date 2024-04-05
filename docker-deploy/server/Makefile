CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq -lpthread

all: server

server: main.cpp server.hpp server.cpp database.hpp database.cpp XMLHandler.hpp XMLHandler.cpp pugixml/pugixml.hpp pugixml/pugixml.cpp
	$(CC) $(CFLAGS) -o server main.cpp server.cpp database.cpp XMLHandler.cpp pugixml/pugixml.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o server

clobber:
	rm -f *~ *.o
