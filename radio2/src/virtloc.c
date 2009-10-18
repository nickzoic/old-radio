// $Id: virtloc.c,v 1.2 2009-10-18 12:58:17 nick Exp $

#include "virtloc.h"

#define K_ATTRACT ((energy_t)1ULL)
#define K_REPEL   ((double)8.0E6)

#define INITIAL_PERTURB (10)


void virtloc_init(virtloc_t *virtloc, neigh_id_t id) 
    loc_zero(&virtloc->loc);
    if (id) loc_perturb(&virtloc->loc, INITIAL_PERTURB);
}

unsigned long energy(loc_t loc, neigh_table_t neigh_table) {
    unsigned long energy = 0;
    
    neigh_iter_t *iter = neigh_iter_new(neigh_table);
    while ((neigh_t *n = neigh_iter_next(iter))) {
        if (stratum == 1)
            energy += K_ATTRACT * loc_dist2(&loc, &n->loc);
        else
            energy += K_REPEL / (loc_dist(&loc, &n->loc)+1);    

        energy += energy(&loc, &n->loc, n->stratum);            
    }
    
    return energy;
}

void virtloc_recalc(virtloc_t *virtloc, neigh_table_t neigh_table) {
    
    unsigned long oldenergy = energy(virtloc->loc, neigh_table);
    
    for (int i=0; i<500; i++) {
        loc_t newloc = virtloc->loc;
        vloc_perturb(newloc, 2000/(i+5));
        
        unsigned long newenergy = energy(newloc, neigh_table);
        if (newenergy <= oldenergy) {
            virtloc->loc = newloc;
            oldenergy = newenergy;
        }
    }
    
}
