CC=gcc
CFLAGS=-I.
DEPS=shellFunctions.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

biplavShell1: biplavShell1.o shellFunctions.o
	$(CC) -o biplavShell1 biplavShell1.o shellFunctions.o -I.