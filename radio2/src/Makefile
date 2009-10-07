# $Id: Makefile,v 1.3 2009-10-07 21:25:54 nick Exp $

CC=gcc
CFLAGS=-Wall -std=c99 -fgnu89-inline -O6 -pg
LDFLAGS=-lm -pg

OBJECTS=vtime.o packet.o queue.o
OLDOBJECTS=loc.o energy.o neigh.o node.o beacon.o

all: $(OBJECTS)

clean:
	rm $(OBJECTS)
    

vtime.o: vtime.c vtime.h

packet.o: packet.c packet.h

queue.o: queue.c queue.h vtime.h node.h packet.h

loc.o: loc.c loc.h

energy.o: energy.c energy.h loc.h

neigh.o: neigh.c neigh.h loc.h vtime.h

node.o: node.c node.h neigh.h loc.h vtime.h beacon.h