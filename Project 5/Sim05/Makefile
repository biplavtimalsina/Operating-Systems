CC=g++ -pthread
CFLAGS=-c -Wall

all: project

project: ConfigFile.o Task.o Process.o Functions.o MemoryFunction.o main.o
	$(CC)  ConfigFile.cpp Task.cpp Process.cpp Functions.cpp MemoryFunction.cpp main.cpp -o output1

Config.o: ConfigFile.cpp
	$(CC) $(CFLAGS) ConfigFile.cpp

Task.o:Task.cpp
	$(CC) $(CFLAGS) Task.cpp

Process.o:Process.cpp
	$(CC) $(CFLAGS) Process.cpp


Functions.o:Functions.cpp
	$(CC) $(CFLAGS) Functions.cpp
MemoryFunction.o:
	$(CC) $(CFLAGS) MemoryFunction.cpp
main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *o output

