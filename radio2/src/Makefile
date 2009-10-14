# $Id: Makefile,v 1.8 2009-10-14 05:31:16 nick Exp $

CC=gcc
CFLAGS=-Wall -std=c99 -fgnu89-inline -O6 -pg
LDFLAGS=-lm -pg

OBJECTS_BOTH=vtime.o loc.o packet.o node.o neigh.o
OBJECTS_SIM=topo.o sim.o queue.o
OBJECTS_USB=
OBJECTS_ALL=$(OBJECTS_BOTH) $(OBJECTS_SIM) $(OBJECTS_USB)

all: sim

clean:
	rm $(OBJECTS_ALL)

sim: $(OBJECTS_BOTH) $(OBJECTS_SIM)


vtime.o: vtime.c vtime.h

loc.o: loc.c loc.h

packet.o: packet.c packet.h

queue.o: queue.c queue.h vtime.h node.h packet.h

node.o: node.c node.h neigh.h loc.h vtime.h packet.h

neigh.o: neigh.c neigh.h loc.h vtime.h

energy.o: energy.c energy.h loc.h

topo.o: topo.c topo.h node.h

