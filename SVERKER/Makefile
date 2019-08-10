CC=gcc
CPP=g++
CFLAGS=-I/usr/include/sfml1/
LIBS=-lsfml-network-1.6
OPTS=-Wall -std=c++11
OBJ=main.o config.o stringHandler.o IRCConnection.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(OPTS)

sverkerbot: $(OBJ)
	$(CPP) -o sverkerbot $(LIBS) $(OPTS) $(OBJ)

clean:
	rm -f *.o sverkerbot

#gcc -c -I/usr/include/sfml1/ main.cpp

#g++ -lsfml-system-1.6 -lsfml-network-1.6 main.o -o sverkerbot
