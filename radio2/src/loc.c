// $Id: loc.c,v 1.7 2009-11-16 06:40:32 nick Exp $

// Abstract away the concept of a "location".
// This could pretty easily be extended to be N-dimensional rather than always 3.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "loc.h"

////////////////////////////////////////////////////////////////////////////////

unsigned long loc_dist2(loc_t *a, loc_t *b) {
    long dx = b->x - a->x;
    long dy = b->y - a->y;
    long dz = b->z - a->z;
    
    return dx*dx + dy*dy + dz*dz;
}

////////////////////////////////////////////////////////////////////////////////

double loc_dist(loc_t *a, loc_t *b) {
    return sqrt(loc_dist2(a,b));
}

////////////////////////////////////////////////////////////////////////////////

void loc_zero(loc_t *a) {
    a->x = 0;
    a->y = 0;
    a->z = 0;
}

////////////////////////////////////////////////////////////////////////////////

void loc_perturb(loc_t *a, unsigned int d) {
    a->x += (rand()%(2*d+1)) - d;
    a->y += (rand()%(2*d+1)) - d;
    a->z += (rand()%(2*d+1)) - d;
}

////////////////////////////////////////////////////////////////////////////////

void loc_fprint(loc_t *a, FILE *fp) {
    fprintf(fp, " %+6d %+6d %+6d ", a->x, a->y, a->z);
}

////////////////////////////////////////////////////////////////////////////////

void loc_move_towards(loc_t *a, loc_t *b, unsigned int dist_limit) {
    // move location a towards location b, limited to dist_limit distance.
    long dx = b->x - a->x;
    long dy = b->y - a->y;
    long dz = b->z - a->z;
    
    unsigned long dist2 = dx*dx + dy*dy + dz*dz;
    unsigned long dist_limit2 = dist_limit * dist_limit;
    if (dist_limit2 >= dist2) {
        *a = *b;
    } else {
        float frac = sqrtf((float)dist_limit2 / dist2);
        a->x += dx * frac;
        a->y += dy * frac;
        a->z += dz * frac;
    }
}
