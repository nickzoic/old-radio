// $Id: neigh.h,v 1.7 2009-10-09 10:00:13 nick Exp $

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
} __attribute__((__packed__)) neigh_t;

typedef struct neigh_entry_s {
    neigh_t neigh;
    vtime_t expiry;
} neigh_entry_t;

typedef struct neigh_table_s {
    neigh_entry_t table[NEIGH_ID_MAX+1];
} neigh_table_t;

void neigh_table_insert(neigh_table_t *neigh_table, neigh_t neigh, vtime_t vtime);

#endif