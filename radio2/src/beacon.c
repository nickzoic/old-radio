// $Id: beacon.c,v 1.1 2009-09-24 00:21:13 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "vtime.h"
#include "loc.h"
#include "neigh.h"
#include "beacon.h"

void beacon_recv(beacon_t *beacon, neigh_tab_t *neigh_tab, vtime_t vtime) {    
    while (0) {
        neigh_t neigh;
        neigh.vtime = vtime;
        
        neigh_tab_insert(neigh_tab, vtime, &neigh);
    }
}

beacon_t *beacon_create(neigh_tab_t *neigh_tab) {
    return NULL;
}

void beacon_destroy(beacon_t *beacon) {
    free(beacon);
}