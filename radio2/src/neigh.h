// $Id: neigh.h,v 1.6 2009-10-08 02:26:53 nick Exp $

#ifndef _NEIGH_H
#define _NEIGH_H

#include <stdint.h>

#include "vtime.h"
#include "loc.h"

#define NEIGH_EXPIRY_TIMEOUT_MS (2500)

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
    neigh_t table[NEIGH_ID_MAX];
} neigh_table_t;

void neigh_table_insert(neigh_table_t *neigh_table, neigh_id_t id,
        neigh_stratum_t stratum, loc_t loc, vtime_t vtime);

#endif