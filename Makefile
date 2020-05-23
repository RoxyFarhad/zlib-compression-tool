
TARGET=main
CLFAGS=-Wall
CC = gcc
CLFAGS = -g -Wall

.PHONY: clean

$(TARGET): main.o zlibfunc.o
	$(CC) -g main.o zlibfunc.o -o $(TARGET) -lz

main.o: main.c zlibfunc.h
	$(CC) -c $(CFLAGS) main.c

zlibfunc.o: zlibfunc.c zlibfunc.h
	$(CC) -c $(CFLAGS) zlibfunc.c

clean:
	rm -f $(TARGET) *.o *.z *.txt.z