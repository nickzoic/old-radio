# $Id: Makefile,v 1.2 2009-10-07 18:50:37 nick Exp $

CC=gcc
CFLAGS=-Wall -std=c99 -fgnu89-inline -O6 -pg
LDFLAGS=-lm -pg

OBJECTS=queue.o vtime.o loc.o energy.o neigh.o node.o beacon.o

all: $(OBJECTS)

event_test: event_test.o $(OBJECTS)

clean:
	rm $(OBJECTS)
    
queue.o: queue.c queue.h node.h

vtime.o: vtime.c vtime.h

loc.o: loc.c loc.h

energy.o: energy.c energy.h loc.h

neigh.o: neigh.c neigh.h loc.h vtime.h

node.o: node.c node.h neigh.h loc.h vtime.h beacon.h