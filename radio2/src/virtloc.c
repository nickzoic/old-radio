// $Id: virtloc.c,v 1.1 2009-10-18 11:06:12 nick Exp $

#include "virtloc.h"

#define K_ATTRACT ((energy_t)1ULL)
#define K_REPEL   ((double)8.0E6)

int attract(loc_t *a, loc_t *b) {
    return K_ATTRACT * loc_dist2(a,b);
}

int repel(loc_t *a, loc_t *b) {
    return K_REPEL / (loc_dist(a,b)+1);
}


void virtloc_recalc(vloc_t vloc, neigh_t *nhood) {
        
    
}
