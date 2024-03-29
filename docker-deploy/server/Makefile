CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq -lpthread

all: test

test: xmltest.cpp database.hpp database.cpp XMLHandler.hpp XMLHandler.cpp pugixml/pugixml.hpp pugixml/pugixml.cpp
	$(CC) $(CFLAGS) -o test xmltest.cpp database.cpp XMLHandler.cpp pugixml/pugixml.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
