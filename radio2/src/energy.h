// $Id: energy.h,v 1.3 2009-10-07 18:50:37 nick Exp $

#ifndef _ENERGY_H
#define _ENERGY_H

typedef uint32_t energy_t;

#define ENERGY_K_ATTRACT ((energy_t)1ULL)
#define ENERGY_K_REPEL   ((double)8.0E6)

energy_t energy_attract(loc_t *a, loc_t *b);
energy_t energy_repel(loc_t *a, loc_t *b);

#endif