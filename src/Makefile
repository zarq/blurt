# $Id: Makefile,v 1.3 2001/02/16 20:49:21 zarq Exp $


all: blurt

CFLAGS=-g3 -O0 -Wall -ansi
LDFLAGS=-g -L/usr/X11R6/lib -lX11

blurt: main.o process.o screen.o vt.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f blurt main.o process.o screen.o vt.o
