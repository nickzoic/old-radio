# $Id: Makefile,v 1.7 2009-01-22 01:07:42 nick Exp $

CFLAGS=-std=gnu99 -O2 -Wall

all: send_packets recv_packets test_ber mktestdata send_byte recv_bytes send_bytes test_symbols

symbols.h: symbols.py
	./symbols.py 3 3 3 8 55 AA DD > symbols.h

mktestdata: symbols.h

sendrecv.o: sendrecv.c symbols.h

send_packets: send_packets.o sendrecv.o

recv_packets: recv_packets.o sendrecv.o

send_packets.o: send_packets.c sendrecv.h

recv_packets.o: recv_packets.c sendrecv.h

test_ber.o: test_ber.c sendrecv.h

test_ber: test_ber.o sendrecv.o

send_byte: send_byte.o sendrecv.o

send_bytes: send_bytes.o sendrecv.o

recv_bytes: recv_bytes.o sendrecv.o

test_symbols: test_symbols.o sendrecv.o

clean:
	rm -rf *.o send_packets recv_packets test_ber
    
