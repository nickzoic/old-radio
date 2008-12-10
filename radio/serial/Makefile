# $Id: Makefile,v 1.1 2008-12-10 05:43:58 nick Exp $

CFLAGS = -O2 -Wall

all: send_packets recv_packets test_ber

send_packets: send_packets.o sendrecv.o

recv_packets: recv_packets.o sendrecv.o

test_ber: test_ber.o sendrecv.o

clean:
	rm -rf *.o send_packets recv_packets test_ber
    
