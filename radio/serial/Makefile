# $Id: Makefile,v 1.9 2009-01-27 23:59:18 nick Exp $

CFLAGS=-std=gnu99 -O2 -Wall

all: send_packets recv_packets


test: test_symbols
	./test_symbols
   
 
send_packets: send_packets.o sendrecv.o symbols.o


recv_packets: recv_packets.o sendrecv.o symbols.o


test_symbols: test_symbols.o symbols.o


symbols.i: symbols.py
	./symbols.py 3 3 3 8 55 AA FF DD > symbols.i

symbols.o: symbols.c symbols.h symbols.i


clean:
	rm -rf *.o symbols.i test_symbols
    
