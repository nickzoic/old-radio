# $Id: Makefile,v 1.14 2009-10-18 09:09:33 nick Exp $

CC=gcc

#CFLAGS=-Wall -std=gnu99 -g -pg
#LDFLAGS=-lm -pg

CFLAGS=-Wall -std=gnu99 -O2 -g
LDFLAGS=-lm

OBJECTS_BOTH=vtime.o loc.o packet.o node.o neigh.o
OBJECTS_SIM=topo.o sim.o queue.o
OBJECTS_SERIAL=serial.o radio.o crc.o symbols.o

all: sim serial

test: queue_test symbols_test crc_test
	./queue_test
	./symbols_test
	./crc_test
    
clean:
	rm $(OBJECTS_BOTH) $(OBJECTS_SIM) $(OBJECTS_SERIAL)


sim: $(OBJECTS_BOTH) $(OBJECTS_SIM)

serial: $(OBJECTS_BOTH) $(OBJECTS_SERIAL)

queue_test: queue_test.o queue.o

symbols_test: symbols_test.o symbols.o

crc_test: crc_test.o crc.o


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
