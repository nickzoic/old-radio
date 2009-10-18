// $Id: neigh.c,v 1.11 2009-10-18 12:58:16 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "neigh.h"

////////////////////////////////////////////////////////////////////////////////

void neigh_table_init(neigh_table_t neigh_table) {
    memset(neigh_table, 0, sizeof(neigh_table_t));
}

////////////////////////////////////////////////////////////////////////////////

void neigh_table_insert(neigh_table_t neigh_table, neigh_t neigh, vtime_t vtime) {
    
    // if we've already got this ID, THEN:
    //     if the new one is the same or lower stratum or has expired, replace it.
    //     otherwise ignore it.
    for (int i=0; i<NEIGH_MAX; i++) {
        if (neigh_table[i].neigh.id == neigh.id) {
            if (neigh_table[i].neigh.stratum <= neigh.stratum ||
                    neigh_table[i].expiry < vtime) {
                neigh_table[i].neigh = neigh;
                neigh_table[i].expiry = vtime + NEIGH_EXPIRY_TIMEOUT;
            }
            return;
        }
    }
    
    // if we haven't got this ID, insert it replacing an expired or empty entry.
    for (int i=0; i<NEIGH_MAX; i++) {
        if (neigh_table[i].expiry < vtime) {
            neigh_table[i].neigh = neigh;
            neigh_table[i].expiry = vtime + NEIGH_EXPIRY_TIMEOUT;
            return;
        }
    }
    
    fprintf(stderr, "Neighbourhood overflow!\n");
}

////////////////////////////////////////////////////////////////////////////////

void neigh_table_cull(neigh_table_t neigh_table, vtime_t vtime) {
    if (neigh_table[i].expiry >= vtime) {
        neigh_table[i].expiry = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void *neigh_table_foreach(neigh_table_t neigh_table, void *(*func)(neigh_t neigh, void *data), void *data) {
    for (int i=0; i < NEIGH_MAX; i++) {
        if (neigh_table[i].expiry) data = func(neigh_table[i].neigh, data);
    }
    return data;
}

////////////////////////////////////////////////////////////////////////////////

neigh_iter_t *neigh_iter_new(neigh_table_t neigh_table) {
    neigh_iter_t *iter = (neigh_iter_t *)malloc(sizeof(neigh_iter_t));
    iter->table = neigh_table;
    iter->index = 0;
    return iter;
}

////////////////////////////////////////////////////////////////////////////////

neigh_t *neigh_iter_next(neigh_iter_t *iter) {
    while(iter->index < NEIGH_MAX) {
        neigh_entry_t *entry = &iter->table[iter->index];
        iter->index++;
        if (entry->expiry) return &entry->neigh;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void neigh_iter_free(neigh_iter_t *iter) {
    free(iter);
}
