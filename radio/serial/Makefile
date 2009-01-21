# $Id: Makefile,v 1.5 2009-01-21 07:22:50 nick Exp $

CFLAGS = -O2 -Wall

all: send_packets recv_packets test_ber mktestdata send_byte recv_bytes send_bytes

send_packets: send_packets.o sendrecv.o

recv_packets: recv_packets.o sendrecv.o

send_packets.o: send_packets.c sendrecv.h

recv_packets.o: recv_packets.c sendrecv.h

test_ber.o: test_ber.c sendrecv.h

test_ber: test_ber.o sendrecv.o

send_byte: send_byte.o sendrecv.o

send_bytes: send_bytes.o sendrecv.o

recv_bytes: recv_bytes.o sendrecv.o

clean:
	rm -rf *.o send_packets recv_packets test_ber
    
