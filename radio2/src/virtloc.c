// $Id: virtloc.c,v 1.6 2009-10-19 20:07:58 nick Exp $

#include "virtloc.h"

#define K_ATTRACT (1ULL)
#define K_REPEL   ((double)8.0E6)

#define INITIAL_PERTURB (10)


void virtloc_init(virtloc_t *virtloc, neigh_id_t id) {
    loc_zero(&virtloc->loc);
    if (id) loc_perturb(&virtloc->loc, INITIAL_PERTURB);
}

unsigned long energy(loc_t loc, neigh_table_t *neigh_table) {
    unsigned long energy = 0;
    int valid = 0;
    
    neigh_iter_t *iter = neigh_iter_new(neigh_table);
    neigh_t *n;
    while (( n = neigh_iter_next(iter) )) {
        //printf("$$$ %d\n", n->id);
        if (n->stratum == 1) {
            energy += K_ATTRACT * loc_dist2(&loc, &n->loc);
            valid = 1;
        } else if (n->stratum >= 2) {
            energy += K_REPEL / (loc_dist(&loc, &n->loc)+1);    
        }
    }
    
    neigh_iter_free(iter);
    return valid ? energy : 0;
}

void virtloc_recalc(virtloc_t *virtloc, neigh_table_t *neigh_table) {
    
    unsigned long oldenergy = energy(virtloc->loc, neigh_table);
    if (!oldenergy) return;
    
    for (int i=1; i<300; i++) {
        loc_t newloc = virtloc->loc;
        loc_perturb(&newloc, (200/i)+1);
        
        unsigned long newenergy = energy(newloc, neigh_table);
        if (newenergy <= oldenergy) {
            virtloc->loc = newloc;
            oldenergy = newenergy;
        }
    }
}
