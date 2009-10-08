// $Id: neigh.c,v 1.5 2009-10-08 00:01:44 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "neigh.h"

int neigh_more_important(neigh_t *a, neigh_t *b) {
    if (b->status == 0) return 1;
    //if (a->stratum > b->stratum) return 0;
    return 1;
}

void neigh_table_insert(neigh_table_t *neigh_table, vtime_t vtime, neigh_t *neigh) {
 
}