// $Id: beacon.c,v 1.11 2009-08-12 03:49:37 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <math.h>

#include <sys/time.h>

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
	Neighbours[0].vloc[i] = rand() % 11 - 5;
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
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long tt = tv.tv_sec*1000000+tv.tv_usec;
    
    // printf("%lu %d R %d\n", tt, Identifier, beacon[0].id);
    
    // If our id isn't in the packet, maybe we can't be heard by this node
    for (i=1; i<nbeacon; i++) {
        if (beacon[i].id == Identifier && (beacon[i].stratum == 1 || beacon[i].stratum == STRAT_INF)) break;
    }
    if (i == nbeacon) {
        // our id wasn't found ... truncate the packet.
        nbeacon = 1;
        beacon[0].stratum = STRAT_INF;
    }
    
    // printf("----- RECV %6d %d\n", beacon[0].id, nbeacon);
    
    for (i=0; i<nbeacon; i++) {
        if (beacon[i].id == Identifier) continue;
        
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
	}
    }
}

int beacon_prepare(unsigned char *buffer, int length) {
    beacon_t *beacon = (beacon_t *)buffer;
    int j = 0;
    for (int i=0; i<Nneigh && j * sizeof(beacon_t) < length; i++) {
	if (Neighbours[i].state == 0) continue;
	if (Neighbours[i].stratum >= MAXSTRAT && Neighbours[i].stratum != STRAT_INF) continue;
	
	beacon[j].id = Neighbours[i].id;
	beacon[j].stratum = Neighbours[j].stratum;
	for (int k=0; k<VLOC_DIM; k++) beacon[j].vloc[k] = Neighbours[i].vloc[k];
	j++;
    }
    
    return j * sizeof(beacon_t);
}


unsigned long calc_energy(vloc_t vloc) {
    unsigned long ener = 0;
    for (int i=1; i<Nneigh; i++) {
	
	if (!Neighbours[i].state) continue;
	strat_t stratum = Neighbours[i].stratum;
	if (!stratum || stratum == STRAT_INF) continue;
	
	unsigned long dist2 = 0;
	for (int j=0; j<VLOC_DIM; j++) {
	    long dd = Neighbours[i].vloc[j] - vloc[j];
	    dist2 += dd * dd;
	}
	
	if (stratum == 1) {
	    ener += K_ATTRACT * dist2;
	} else if (stratum <= MAXSTRAT) {
	    ener += K_REPEL / (sqrt(dist2) + 1);
	}
    }
    return ener;
}

void beacon_recalc() {
    time_t stamp_timeout = time(NULL) - BEACON_TIMEOUT;
    
    for (int i=1; i<Nneigh; i++)
	if (Neighbours[i].stamp < stamp_timeout)
	    Neighbours[i].state = 0;
    
    unsigned long curenergy = calc_energy(Neighbours[0].vloc);
    
    vloc_t oldvloc;
    for (int k=0; k<VLOC_DIM; k++) oldvloc[k] = Neighbours[0].vloc[k];
    unsigned long oldenergy = curenergy;
    
    for (int i=0; i<100; i++) {
	vloc_t newvloc;
	for (int k=0; k<VLOC_DIM; k++) newvloc[k] = oldvloc[k] + (rand() % 3) - 1;
	
	unsigned long newenergy = calc_energy(newvloc);
	if (newenergy <= oldenergy) {
	    for (int k=0; k<VLOC_DIM; k++) oldvloc[k] = newvloc[k];
	    oldenergy = newenergy;
	}
    }
    if (oldenergy < curenergy) {
	for (int k=0; k<VLOC_DIM; k++) Neighbours[0].vloc[k] = oldvloc[k];
    }
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long tt = (tv.tv_sec % 1000)*1000000+tv.tv_usec;
    
    printf("%lu %d %+5d %+5d %+5d  ", tt, Identifier, Neighbours[0].vloc[0], Neighbours[0].vloc[1], Neighbours[0].vloc[2]);
    
    for (int i=1; i<Nneigh; i++) {
	if (!Neighbours[i].state || Neighbours[i].stratum != 1) continue;
	printf (" %d", Neighbours[i].id);
    }
    printf("\n");
}
