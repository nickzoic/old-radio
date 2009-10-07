// $Id: neigh.c,v 1.4 2009-10-07 18:50:38 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "neigh.h"

int neigh_more_important(neigh_t *a, neigh_t *b) {
    if (b->status == 0) return 1;
    //if (a->stratum > b->stratum) return 0;
    return 1;
}

void neigh_tab_insert(neigh_tab_t *neigh_tab, vtime_t vtime, neigh_t *neigh) {
    if (neigh->status <= neigh_tab[neigh->id]->status) {
        memcpy(&neigh_tab[neigh->id], neigh, sizeof(neigh_t));
    }
}