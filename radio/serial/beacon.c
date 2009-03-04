// $Id: beacon.c,v 1.5 2009-03-04 07:14:40 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "beacon.h"

nodeid_t Identifier = 0;
int Nneigh = 0;
neighbour_t Neighbours[MAXNEIGH] = { { 0 } };

void print_neigh(neighbour_t *neigh) {
    printf("%d (%d) [ ", neigh->id, neigh->stratum);
    for (int i = 0; i < VLOC_DIM; i++) {
	printf("%d ", neigh->vloc[i]);
    }
    printf("]\n");
}

void beacon_init(nodeid_t identifier) {
    Identifier = identifier;
    Nneigh = 1;
    Neighbours[0].state = 1;
    Neighbours[0].id = Identifier;
    Neighbours[0].stratum = 0;
    for (int i = 0; i < VLOC_DIM; i++) {
	Neighbours[0].vloc[i] = rand() % 255 - 127;
    }
}

void beacon_recv(unsigned char *buffer, int length) {
    
    // Interpret the packet as an array of beacon_t.
    beacon_t *beacon = (beacon_t *)buffer;
    assert(length % sizeof(beacon_t) == 0);
    assert(beacon[0].stratum == 0);
    int nbeacon = length / sizeof(beacon_t);
    int i;
    
    time_t stamp = time(NULL);
    
    // Ignore packets from ourself
    if (beacon[0].id == Identifier) return;
    
    // If our id isn't in the packet, maybe we can't be heard by this node
    for (i=1; i<nbeacon; i++) {
        if (beacon[i].id == Identifier && (beacon[i].stratum == 1 || beacon[i].stratum == STRAT_INF)) break;
    }
    if (i == nbeacon) {
        // our id wasn't found ... truncate the packet.
        nbeacon = 1;
        beacon[0].stratum = STRAT_INF;
    }
    
    printf("----- RECV %6d %d\n", beacon[0].id, nbeacon);
    
    for (i=0; i<nbeacon; i++) {
        if (beacon[i].id == Identifier) continue;
        // if (beacon[i].stratum >= MAXSTRAT && beacon[i].stratum != STRAT_INF) continue;
        
	//printf("%d: %6d (%d) [%d %d %d]", i, beacon[i].id, beacon[i].stratum,
	//       beacon[i].vloc[0], beacon[i].vloc[1], beacon[i].vloc[2]);
	
        int j;
        for (j=0; j < Nneigh; j++) {
            if (Neighbours[j].id == beacon[i].id) break;
        }
        assert (j < MAXNEIGH);
	
	strat_t stratum = beacon[i].stratum;
	if (stratum < STRAT_INF) stratum++;
	
        if (j == Nneigh || !Neighbours[j].state || Neighbours[j].stratum >= stratum) {
            if (j == Nneigh) Nneigh++;
	    
	    Neighbours[j].id = beacon[i].id;
	    Neighbours[j].stratum = stratum;
	    
	    for (int k=0; k<VLOC_DIM; k++) Neighbours[j].vloc[k] = beacon[i].vloc[k];
        
	    Neighbours[j].stamp = stamp;
	    Neighbours[j].state = 1;
	    
	    //printf(" => %d", j);
        }
	//printf("\n");
    }
}

int beacon_prepare(unsigned char *buffer, int length) {
    time_t stamp_timeout = time(NULL) - 3;
    beacon_t *beacon = (beacon_t *)buffer;
    int j = 0;
    printf("----- TABLE %6d %d\n", Identifier, Nneigh);
    for (int i=0; i<Nneigh && j * sizeof(beacon_t) < length; i++) {
	
	printf("%d {%d}: %6d (%d) [%d %d %d]\n", i, Neighbours[i].state, Neighbours[i].id, Neighbours[i].stratum,
	       Neighbours[i].vloc[0], Neighbours[i].vloc[1], Neighbours[i].vloc[2]);
	
	if (Neighbours[i].state == 0) continue;
	if (Neighbours[i].stratum != 0 && Neighbours[i].stamp < stamp_timeout) {
	    Neighbours[i].state = 0;
	    continue;
	}
	
	
	if (Neighbours[i].stratum >= MAXSTRAT && Neighbours[i].stratum != STRAT_INF) continue;
	
	beacon[j].id = Neighbours[i].id;
	beacon[j].stratum = Neighbours[j].stratum;
	for (int k=0; k<VLOC_DIM; k++) beacon[j].vloc[k] = Neighbours[i].vloc[k];
	j++;
    }
    printf("----- SEND %6d %d\n", Identifier, j);
    
    return j * sizeof(beacon_t);
}
