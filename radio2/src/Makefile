# $Id: Makefile,v 1.19 2009-11-17 03:51:43 nick Exp $

CC=gcc

#CFLAGS=-Wall -std=gnu99 -g -pg
#LDFLAGS=-lm -pg

CFLAGS=-Wall -std=gnu99 -O2 -g
LDFLAGS=-lm

OBJECTS_BOTH=vtime.o loc.o packet.o node.o neigh.o virtloc.o
OBJECTS_SIM=topo.o queue.o
OBJECTS_SERIAL=radio.o crc.o symbols.o

################################################################################

all: sim serial topostats

test: queue_test symbols_test crc_test packet_test
	./queue_test
	./symbols_test
	./crc_test
	./packet_test
    
clean:
	rm $(OBJECTS_BOTH) $(OBJECTS_SIM) $(OBJECTS_SERIAL)

################################################################################

sim: sim.o $(OBJECTS_BOTH) $(OBJECTS_SIM)

serial: serial.o $(OBJECTS_BOTH) $(OBJECTS_SERIAL)

topostats: topostats.o $(OBJECTS_BOTH) $(OBJECTS_SERIAL)

queue_test: queue_test.o queue.o

symbols_test: symbols_test.o symbols.o

packet_test: packet_test.o packet.o

crc_test: crc_test.o crc.o

################################################################################

vtime.o: vtime.c vtime.h

loc.o: loc.c loc.h

packet.o: packet.c packet.h

queue.o: queue.c queue.h vtime.h node.h packet.h

node.o: node.c node.h neigh.h loc.h vtime.h packet.h

neigh.o: neigh.c neigh.h loc.h vtime.h

energy.o: energy.c energy.h loc.h

topo.o: topo.c topo.h node.h

crc.o: crc.c crc.h

symbols.o: symbols.c symbols.h symbols.i

radio.o: radio.c radio.h symbols.h crc.h

serial.o: serial.c vtime.h node.h radio.h

virtloc.o: virtloc.c virtloc.h loc.h vtime.h neigh.h node.h