// $Id: neigh.h,v 1.5 2009-10-08 00:01:44 nick Exp $

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

typedef struct neigh_table_s {
    neigh_t *stratum[2];    
} neigh_table_t;

void neigh_table_insert(neigh_table_t *neigh_table, vtime_t vtime, neigh_t *neigh);

#endif