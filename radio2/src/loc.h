// $Id: loc.h,v 1.2 2009-09-24 03:01:34 nick Exp $

#ifndef _LOC_H
#define _LOC_H

typedef int16_t locx_t;

typedef struct loc_s {
    locx_t x, y, z;
} loc_t;

unsigned long loc_dist2(loc_t *a, loc_t *b);
double loc_dist(loc_t *a, loc_t *b);
void loc_zero(loc_t *a);
void loc_perturb(loc_t *a, unsigned int d);
void loc_fprint(loc_t *a, FILE *fp);

#endif