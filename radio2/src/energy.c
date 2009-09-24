// $Id: energy.c,v 1.2 2009-09-24 03:01:34 nick Exp $

// There are two types of "energy" used in VL ... attraction to 1-neighbours
// and repulsion from 2-neighbours.

#include <stdio.h>
#include <stdint.h>

#include "loc.h"
#include "energy.h"

energy_t energy_attract(loc_t *a, loc_t *b) {
    return ENERGY_K_ATTRACT * loc_dist2(a,b);
}

energy_t energy_repel(loc_t *a, loc_t *b) {
    return ENERGY_K_REPEL / (loc_dist(a,b)+1);
}