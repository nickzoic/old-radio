// $Id: neigh.c,v 1.9 2009-10-18 05:28:06 nick Exp $

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "neigh.h"

void neigh_table_insert(neigh_table_t *neigh_table, neigh_t neigh, vtime_t vtime) {
    assert(neigh.id <= NEIGH_ID_MAX);
    neigh_entry_t *entry = &(neigh_table->table[neigh.id]);
    
    if ( (entry->expiry < vtime) || (entry->neigh.stratum > neigh.stratum) ) {
        entry->neigh = neigh;
        entry->expiry = vtime + NEIGH_EXPIRY_TIMEOUT;
    }    
}

void *neigh_table_foreach(neigh_table_t *neigh_table, void *(*func)(neigh_t neigh, void *data), void *data) {
    for (int i=0; i <= NEIGH_ID_MAX; i++) {
        data = func(neigh_table->table[i].neigh, data);
    }
    return data;
}

/*neigh_entry_t *neigh_table_iterate(neigh_table_t *neigh_table) {
    return neigh_table->table;
}

neigh_entry_t *neigh_table_iterate_next(neigh_table_t *neigh_table, neigh_entry *neigh_entry) {
    neigh_entry++;
    if (neigh_entry > neigh_table->table + NEIGH_ID_MAX) return NULL;
    return neigh_entry;
}*/

