CC=gcc
CPP=g++
CFLAGS=-I/usr/include/sfml1/
LIBS=-lsfml-system-1.6 -lsfml-network-1.6
OPTS=-Wno-multichar -std=c++11

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(OPTS)

sverkerbot: main.o config.o stringHandler.o IRCConnection.o
	$(CPP) -o sverkerbot $(LIBS) $(OPTS) main.o config.o stringHandler.o IRCConnection.o

clean:
	rm -f *.o sverkerbot

#gcc -c -I/usr/include/sfml1/ main.cpp

#g++ -lsfml-system-1.6 -lsfml-network-1.6 main.o -o sverkerbot
