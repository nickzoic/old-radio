# $Id: Makefile,v 1.9 2009-10-14 07:08:00 nick Exp $

CC=gcc
#CFLAGS=-Wall -std=c99 -pg
CFLAGS=-Wall -std=c99 -O6
LDFLAGS=-lm -pg

OBJECTS_BOTH=vtime.o loc.o packet.o node.o neigh.o
OBJECTS_SIM=topo.o sim.o queue.o
OBJECTS_RADIO=radio.o

all: sim radio

test: queue_test
	./queue_test && echo "PASS"
    
clean:
	rm $(OBJECTS_BOTH) $(OBJECTS_SIM) $(OBJECTS_RADIO)

sim: $(OBJECTS_BOTH) $(OBJECTS_SIM)

radio: $(OBJECTS_BOTH) $(OBJECTS_RADIO)

queue_test: queue_test.o queue.o

vtime.o: vtime.c vtime.h

loc.o: loc.c loc.h

packet.o: packet.c packet.h

queue.o: queue.c queue.h vtime.h node.h packet.h

node.o: node.c node.h neigh.h loc.h vtime.h packet.h

neigh.o: neigh.c neigh.h loc.h vtime.h

energy.o: energy.c energy.h loc.h

topo.o: topo.c topo.h node.h

