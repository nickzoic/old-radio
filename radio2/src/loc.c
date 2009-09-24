// $Id: loc.c,v 1.2 2009-09-24 03:01:34 nick Exp $

// Abstract away the concept of a "location".
// This could pretty easily be extended to be N-dimensional rather than always 3.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "loc.h"

unsigned long loc_dist2(loc_t *a, loc_t *b) {
    long dx = a->x - b->x;
    long dy = a->y - b->y;
    long dz = a->z - b->z;
    
    return dx*dx + dy*dy + dz*dz;
}

double loc_dist(loc_t *a, loc_t *b) {
    return sqrt(loc_dist2(a,b));
}

void loc_zero(loc_t *a) {
    a->x = 0;
    a->y = 0;
    a->z = 0;
}

void loc_perturb(loc_t *a, unsigned int d) {
    a->x += (rand()%(2*d)) - d;
    a->y += (rand()%(2*d)) - d;
    a->z += (rand()%(2*d)) - d;
}

void loc_fprint(loc_t *a, FILE *fp) {
    fprintf(fp, " %+6d %+6d %+6d ", a->x, a->y, a->z);
}