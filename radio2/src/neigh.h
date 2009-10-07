// $Id: neigh.h,v 1.4 2009-10-07 18:50:38 nick Exp $

#ifndef _NEIGH_H
#define _NEIGH_H

#include <stdint.h>

#include "vtime.h"
#include "loc.h"

#define NEIGH_ID_MAX (50)

typedef uint8_t neigh_id_t;
typedef uint8_t neigh_stratum_t;

typedef struct neigh_s {
    neigh_id_t id;
    neigh_stratum_t stratum;
    loc_t loc;
    vtime_t vtime;
} neigh_t;

typedef neigh_t neigh_tab_t[NEIGH_ID_MAX+1]; 

void neigh_tab_insert(neigh_tab_t *neigh_tab, vtime_t vtime, neigh_t *neigh);

#endif