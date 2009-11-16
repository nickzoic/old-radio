// $Id: neigh.c,v 1.17 2009-11-16 01:42:07 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "neigh.h"

///////////////////////////////////////////////////////////////  neigh_table_new

neigh_table_t *neigh_table_new() {
    return (neigh_table_t *)calloc(1, sizeof(neigh_table_t));
}

////////////////////////////////////////////////////////////  neigh_table_insert

void neigh_table_insert(neigh_table_t *neigh_table, neigh_t neigh, vtime_t vtime) {
    
    // if we've already got this ID, THEN:
    //     if the new one is the same or lower stratum or has expired, replace it.
    //     otherwise ignore it.
    for (int i=0; i<neigh_table->nneigh; i++) {
        if (neigh_table->table[i].neigh.id == neigh.id) {
            if (neigh_table->table[i].neigh.stratum >= neigh.stratum) {
                neigh_table->table[i].neigh = neigh;
                neigh_table->table[i].expiry = vtime + NEIGH_EXPIRY_TIMEOUT;
            }
            return;
        }
    }
    
    assert(neigh_table->nneigh < NEIGH_MAX);
    
    neigh_table->table[neigh_table->nneigh].neigh = neigh;
    neigh_table->table[neigh_table->nneigh].expiry = vtime + NEIGH_EXPIRY_TIMEOUT;
    neigh_table->nneigh++;
}

//////////////////////////////////////////////////////////////  neigh_table_cull

void neigh_table_cull(neigh_table_t *neigh_table, vtime_t vtime) {
    for (int i=0; i<neigh_table->nneigh; i++) {
        if (neigh_table->table[i].expiry >= vtime) {
            neigh_table->table[i].expiry = 0;
        }
    }
}

///////////////////////////////////////////////////////////  neigh_table_foreach

void *neigh_table_foreach(neigh_table_t *neigh_table, void *(*func)(neigh_t neigh, void *data), void *data) {
    for (int i=0; i < neigh_table->nneigh; i++) {
        if (neigh_table->table[i].expiry) data = func(neigh_table->table[i].neigh, data);
    }
    return data;
}

////////////////////////////////////////////////////////////////  neigh_iter_new

neigh_iter_t *neigh_iter_new(neigh_table_t *neigh_table) {
    neigh_iter_t *iter = (neigh_iter_t *)malloc(sizeof(neigh_iter_t));
    iter->table = neigh_table;
    iter->index = 0;
    return iter;
}

///////////////////////////////////////////////////////////////  neigh_iter_next

neigh_t *neigh_iter_next(neigh_iter_t *iter) {
    while(iter->index < iter->table->nneigh) {
        neigh_entry_t *entry = &iter->table->table[iter->index];
        iter->index++;
        if (entry->expiry) return &entry->neigh;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////  neigh_iter_free

void neigh_iter_free(neigh_iter_t *iter) {
    free(iter);
}

//////////////////////////////////////////////////////////////  neigh_table_free

void neigh_table_free(neigh_table_t *table) {
    free(table);
}

////////////////////////////////////////////////////////////////////////////////