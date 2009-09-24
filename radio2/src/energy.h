// $Id: energy.h,v 1.2 2009-09-24 03:01:34 nick Exp $

#ifndef _ENERGY_H
#define _ENERGY_H

typedef uint32_t energy_t;

#define ENERGY_K_ATTRACT ((energy_t)1ULL)
#define ENERGY_K_REPEL   ((energy_t)8000000ULL)

energy_t energy_attract(loc_t *a, loc_t *b);
energy_t energy_repel(loc_t *a, loc_t *b);

#endif