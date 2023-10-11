CC=gcc
CFLAGS=-Wall -g

all: a3_malloc all

a3_malloc: a3_malloc.o
	$(CC) $(CFLAGS) -o a3_malloc a3_malloc.o

a3_malloc.o: a3_malloc.c a3_malloc.h
	$(CC) $(CFLAGS) -c a3_malloc.c -o a3_malloc.o

test: test.c
	$(CC) $(CFLAGS) -o test test.c

.PHONY: clean

clean:
	- rm -f *.o
	- find . -executable -type f \( ! -name "Makefile" \) -delete