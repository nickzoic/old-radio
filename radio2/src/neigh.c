// $Id: neigh.c,v 1.2 2009-09-24 03:01:34 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "neigh.h"

void neigh_tab_insert(neigh_tab_t *neigh_tab, vtime_t vtime, neigh_t *neigh) {
    if (neigh->status <= neigh_tab[neigh->id]->status) {
        memcpy(&neigh_tab[neigh->id], neigh, sizeof(neigh_t));
    }
}

void neigh_tab_cull(neigh_tab_t *neigh_tab, vtime_t vtime) {
    for (int i=0; i<=NEIGH_ID_MAX; i++) {
        if (neigh_tab[i]->vtime < vtime) {
            neigh_tab[i]->status = 0;
        }
    }
}

energy_t neigh_tab_energy()