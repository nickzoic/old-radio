# $Id: Makefile,v 1.8 2009-01-27 21:55:52 nick Exp $

CFLAGS=-std=gnu99 -O2 -Wall

all: sendrecv.o

symbols.i: symbols.py
	./symbols.py 3 3 3 8 55 AA DD > symbols.i

symbols.o: symbols.c symbols.h symbols.i

clean:
	rm -rf *.o send_packets recv_packets test_ber
    
