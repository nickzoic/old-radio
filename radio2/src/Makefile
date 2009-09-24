# $Id: Makefile,v 1.1 2009-09-24 00:21:13 nick Exp $

CC=gcc
CFLAGS=-Wall -std=c99 -fgnu89-inline -O6 -pg
LDFLAGS=-lm -pg

OBJECTS=vtime.o loc.o energy.o neigh.o node.o beacon.o event.o

all: $(OBJECTS)

event_test: event_test.o $(OBJECTS)

clean:
	rm $(OBJECTS)
    

vtime.o: vtime.c vtime.h

loc.o: loc.c loc.h

energy.o: energy.c energy.h loc.h

neigh.o: neigh.c neigh.h loc.h vtime.h

node.o: node.c node.h neigh.h loc.h vtime.h beacon.h

event.o: event.c event.h loc.h beacon.h neigh.h node.h