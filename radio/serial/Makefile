# $Id: Makefile,v 1.13 2009-02-05 01:48:07 nick Exp $

CFLAGS=-std=gnu99 -O2 -Wall

PROGS=send_packets recv_packets chatter

all: ${PROGS}


test: test_symbols
	./test_symbols
   

chatter: chatter.o sendrecv.o symbols.o

chatter.o: chatter.c sendrecv.h

 
send_packets: send_packets.o sendrecv.o symbols.o sendrecv_packets.h

send_packets.o: send_packets.c sendrecv.h sendrecv_packets.h


recv_packets: recv_packets.o sendrecv.o symbols.o sendrecv_packets.h

recv_packets.o: recv_packets.c sendrecv.h sendrecv_packets.h


test_symbols: test_symbols.o symbols.o


symbols.i: symbols.py
	./symbols.py 3 3 3 8 55 AA FF DD > symbols.i

symbols.o: symbols.c symbols.h symbols.i


clean:
	rm -rf *.o symbols.i test_symbols ${PROGS}
    
