// $Id: neigh.h,v 1.12 2009-10-19 01:02:13 nick Exp $

#ifndef _NEIGH_H
#define _NEIGH_H

#include <stdint.h>

#include "vtime.h"
#include "loc.h"

#define NEIGH_EXPIRY_TIMEOUT (2500 * VTIME_MILLIS)

#define NEIGH_MAX (50)

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
    neigh_entry_t table[NEIGH_MAX];
    int nneigh;
} neigh_table_t;

typedef struct neigh_iter_s {
    neigh_table_t *table;
    int index;
} neigh_iter_t;

void neigh_table_init(neigh_table_t neigh_table);

void neigh_table_insert(neigh_table_t neigh_table, neigh_t neigh, vtime_t vtime);
void neigh_table_cull(neigh_table_t neigh_table, vtime_t vtime);

neigh_iter_t *neigh_iter_new(neigh_table_t neigh_table);
neigh_t *neigh_iter_next(neigh_iter_t *iter);
void neigh_iter_free(neigh_iter_t *iter);

#endif