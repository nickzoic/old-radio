// $Id: neigh.c,v 1.6 2009-10-08 02:26:53 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "neigh.h"

int neigh_more_important(neigh_t *a, neigh_t *b, vtime_t expiry) {
    if (b->vtime > expiry) return 1;
    if (a->stratum < b->stratum) return 1;
    if (a->vtime > b->vtime) return 1;
    return 0;
}

void neigh_table_insert(neigh_table_t *neigh_table, neigh_id_t id,
                neigh_stratum_t stratum, loc_t loc, vtime_t vtime) {
 
    neigh_t nn = { id, stratum, loc, vtime };
    
    if (neigh_more_important(&nn, &neigh_table->table[id], vtime_add_ms(vtime, NEIGH_EXPIRY_TIMEOUT_MS))) {
        neigh_table->table[id] = nn;
    }
}