# $Id: Makefile,v 1.4 2009-01-20 22:49:19 nick Exp $

CFLAGS = -O2 -Wall

all: send_packets recv_packets test_ber mktestdata send_byte

send_packets: send_packets.o sendrecv.o

recv_packets: recv_packets.o sendrecv.o

send_packets.o: send_packets.c sendrecv.h

recv_packets.o: recv_packets.c sendrecv.h

test_ber.o: test_ber.c sendrecv.h

test_ber: test_ber.o sendrecv.o

send_byte: send_byte.o sendrecv.o

clean:
	rm -rf *.o send_packets recv_packets test_ber
    
