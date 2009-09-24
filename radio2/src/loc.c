// $Id: loc.c,v 1.1 2009-09-24 00:21:15 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "loc.h"

locd_t loc_dist2(loc_t *a, loc_t *b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    int dz = a->z - b->z;
    
    return dx*dx + dy*dy + dz*dz;
}

locd_t loc_dist(loc_t *a, loc_t *b) {
    return sqrt(loc_dist2(a,b));
}

void loc_zero(loc_t *a) {
    a->x = 0;
    a->y = 0;
    a->z = 0;
}

void loc_perturb(loc_t *a, locd_t d) {
    a->x += (rand()%(2*d)) - d;
    a->y += (rand()%(2*d)) - d;
    a->z += (rand()%(2*d)) - d;
}

void loc_fprint(loc_t *a, FILE *fp) {
    fprintf(fp, " %+6d %+6d %+6d ", a->x, a->y, a->z);
}