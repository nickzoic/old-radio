// $Id: beacon.c,v 1.1 2009-02-11 06:36:38 nick Exp $

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "beacon.h"

nodeid_t Identifier = 0;
int Nneigh = 0;
neighbour_t Neighbours[MAXNEIGH];

void beacon_init(nodeid_t identifier) {
    Identifier = identifier;
    Nneigh = 1;
    Neighbours[0].id = Identifier;
    Neighbours[0].stratum = 0;
}

void beacon_recv(unsigned char *buffer, int length) {
    neighbour_t *buff = (neighbour_t *)buffer;
    assert(length % sizeof(neighbour_t) == 0);
    int nbuff = length / sizeof(neighbour_t);
    int i;
    
    // Ignore packets from ourself
    if (buff[0].id == Identifier) return;
    assert(buff[0].stratum == 0);
    
    // If our id isn't in the packet, maybe we can't be heard by this node
    for (i=1; i<nbuff; i++) {
        if (buff[i].id == Identifier) break;
    }
    if (i == nbuff) {
        // our id wasn't found
        nbuff = 1;
        buff[0].stratum = STRAT_INF;
    }
    
    for (i=0; i<nbuff; i++) {
        printf("R%d.%d = %d (%d)\n", buff[0].id, i, buff[i].id, buff[i].stratum);
    
        if (buff[i].id == Identifier) continue;
        if (buff[i].stratum >= MAXSTRAT && buff[i].stratum != STRAT_INF) continue;
        
        int j;
        for (j=0; j < Nneigh; j++) {
            if (Neighbours[j].id == buff[i].id) break;
        }
        assert (j < MAXNEIGH);
        if (j == Nneigh || Neighbours[j].stratum >= buff[i].stratum + 1) {
            if (j == Nneigh) Nneigh++;
            memcpy(Neighbours+j, buff+i, sizeof(neighbour_t));
            if (Neighbours[j].stratum < STRAT_INF) Neighbours[j].stratum ++;
            printf("   -> %d.%d: %d (%d)\n", Identifier, j, Neighbours[j].id, Neighbours[j].stratum);
        }
    }
}

int beacon_prepare(unsigned char *buffer, int length) {
    printf("-----\n");
    for (int i=0; i<Nneigh; i++) {
        printf("S%d.%d: %d (%d)\n", Identifier, i, Neighbours[i].id, Neighbours[i].stratum);
    }
    printf("-----\n");
    int size = Nneigh * sizeof(neighbour_t);
    assert (size <= length);
    memcpy(buffer, Neighbours, size);
    return size;
}
