// $Id: virtloc.c,v 1.10 2009-11-16 06:40:34 nick Exp $

#include "virtloc.h"

#define K_ATTRACT (1ULL)
#define K_REPEL   ((double)8.0E6)

#define INITIAL_PERTURB (10)
#define MOVE_LIMIT (100)

//////////////////////////////////////////////////////////////////  virtloc_init

void virtloc_init(virtloc_t *virtloc, neigh_id_t id) {
    loc_zero(&virtloc->loc);
    if (id) loc_perturb(&virtloc->loc, INITIAL_PERTURB);
}

////////////////////////////////////////////////////////////////////////  energy

double energy(loc_t loc, neigh_table_t *neigh_table) {
    double energy = 0;
    int valid = 0;
    
    neigh_iter_t *iter = neigh_iter_new(neigh_table);
    neigh_t *n;
    while (( n = neigh_iter_next(iter) )) {
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

////////////////////////////////////////////////////////////////  virtloc_recalc

void virtloc_recalc(virtloc_t *virtloc, neigh_table_t *neigh_table) {
    
    loc_t oldloc = virtloc->loc;
    double oldenergy = energy(oldloc, neigh_table);
    if (!oldenergy) return;
    
    for (int i=1; i<1000; i++) {
        loc_t newloc = oldloc;
        loc_perturb(&newloc, (500/i)+1);
        
        double newenergy = energy(newloc, neigh_table);
        if (newenergy <= oldenergy) {
            oldloc = newloc;
            oldenergy = newenergy;
        }
    }
    
    loc_move_towards(&virtloc->loc, &oldloc, MOVE_LIMIT);
}
