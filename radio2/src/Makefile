# $Id: Makefile,v 1.5 2009-10-07 22:06:15 nick Exp $

CC=gcc
CFLAGS=-Wall -std=c99 -fgnu89-inline -O6 -pg
LDFLAGS=-lm -pg

OBJECTS=vtime.o loc.o packet.o queue.o node.o
#OLDOBJECTS=energy.o neigh.o beacon.o

all: $(OBJECTS)

clean:
	rm $(OBJECTS)
    

vtime.o: vtime.c vtime.h

loc.o: loc.c loc.h

packet.o: packet.c packet.h

queue.o: queue.c queue.h vtime.h node.h packet.h

node.o: node.c node.h neigh.h loc.h vtime.h packet.h



energy.o: energy.c energy.h loc.h

neigh.o: neigh.c neigh.h loc.h vtime.h

