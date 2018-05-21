CC=g++
CFLAGS=-c -Wall

all: project

project: configFile.o functions.o main.o
	$(CC)  configFile.cpp functions.cpp main.cpp -o Assignment1

Config.o: configFile.cpp
	$(CC) $(CFLAGS) configFile.cpp

functions.o:functions.cpp
	$(CC) $(CFLAGS) functions.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *o output