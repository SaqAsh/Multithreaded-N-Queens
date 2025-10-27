CC = gcc
CFLAGS = -std=c99 -g -D_GNU_SOURCE -Wall -lpthread


default: all

all: nqueens

nqueens: nqueens.c
	$(CC) $< $(CFLAGS) -onqueens

clean:
	rm nqueens
