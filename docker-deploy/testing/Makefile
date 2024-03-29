CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq -lpthread

all: client

client: main.cpp client.hpp client.cpp
	$(CC) $(CFLAGS) -o client main.cpp client.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o client

clobber:
	rm -f *~ *.o
