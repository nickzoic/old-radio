# $Id: Makefile,v 1.19 2009-02-11 01:10:20 nick Exp $

CFLAGS=-std=gnu99 -O2 -Wall
LDLIBS=-lpthread

PROGS=send_packets recv_packets chatter tchatter

all: ${PROGS}


test: test_symbols test_crc
	./test_symbols
	./test_crc
   

chatter: chatter.o sendrecv.o symbols.o crc.o

chatter.o: chatter.c sendrecv.h


tchatter: tchatter.o sendrecv.o symbols.o crc.o

tchatter.o: tchatter.c sendrecv.h

 
send_packets: send_packets.o sendrecv.o symbols.o sendrecv_packets.h

send_packets.o: send_packets.c sendrecv.h sendrecv_packets.h


recv_packets: recv_packets.o sendrecv.o symbols.o sendrecv_packets.h

recv_packets.o: recv_packets.c sendrecv.h sendrecv_packets.h


test_symbols: test_symbols.o symbols.o

symbols.i: symbols.py
	./symbols.py 3 3 3 8 55 AA FF DD > symbols.i

symbols.o: symbols.c symbols.h symbols.i


test_crc: test_crc.o crc.o


crc.o: crc.c crc.h


clean:
	rm -rf *.o symbols.i test_symbols ${PROGS}
    
