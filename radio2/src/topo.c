// $Id: topo.c,v 1.6 2010-01-31 00:02:21 nick Exp $

#include <assert.h>

#include "topo.h"

#define TOPO_TABLESIZE (30000)
#define TOPO_TABLEOFFS (30)
#define TOPO_MAXENTRIES (9000)

// XXX DOESN'T HANDLE REPEATED ENTRIES CORRECTLY AND ISN'T ALL THAT EFFICIENT

////////////////////////////////////////////////////////////////////////////////

topo_t *topo_new() {
    topo_t *topo = (topo_t *)malloc(sizeof(topo_t));
    topo->tablesize = TOPO_TABLESIZE;
    topo->tableoffs = TOPO_TABLEOFFS;
    topo->table = (topo_entry_t *)calloc(topo->tablesize, sizeof(topo_entry_t));
    topo->entries = 0;
    topo->max_id = 0;
    return topo;
}

////////////////////////////////////////////////////////////////////////////////

void topo_insert(topo_t *topo, topo_entry_t entry) {
    assert(topo->entries < TOPO_MAXENTRIES);
    
    int n = (TOPO_TABLEOFFS * entry.src) % TOPO_TABLESIZE;
    while (topo->table[n].src != topo->table[n].dst) {
        if (topo->table[n].src == entry.src && topo->table[n].dst == entry.dst) {
            // squish duplicates
            topo->table[n] = entry;
            return;
        }
        n = (n + 1) % TOPO_TABLESIZE;
    }
    topo->table[n] = entry;
    topo->entries++;
    
    if (entry.src > topo->max_id) topo->max_id = entry.src;
    if (entry.dst > topo->max_id) topo->max_id = entry.dst;
}

////////////////////////////////////////////////////////////////////////////////

void topo_file_read(topo_t *topo, FILE *fp) {
    char s[1024];
    while(fgets(s, sizeof(s), fp)) {
        int node1, node2;
        float prob12 = 1.0;
        float prob21 = 1.0;
        
        if (sscanf(s, "%d %d %f %f", &node1, &node2, &prob12, &prob21) >= 2) {
            if (prob12 > 0.0) {
                topo_entry_t entry = { node1, node2, prob12 };
                topo_insert(topo, entry);        
            }
            if (prob21 > 0.0) {
                topo_entry_t entry = { node2, node1, prob21 };
                topo_insert(topo, entry);                
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

node_id_t topo_max_id(topo_t *topo) {
    return topo->max_id;
}

////////////////////////////////////////////////////////////////////////////////

void topo_free(topo_t *topo) {
    free(topo->table);
    free(topo);
}

////////////////////////////////////////////////////////////////////////////////

topo_iter_t *topo_iter_new(topo_t *topo, node_id_t src) {
    topo_iter_t *iter = (topo_iter_t *)malloc(sizeof(topo_iter_t));
    iter->topo = topo;
    iter->src = src;
    iter->offs = (TOPO_TABLEOFFS * src) % TOPO_TABLESIZE;
    return iter;
}

////////////////////////////////////////////////////////////////////////////////

topo_entry_t *topo_iter_next(topo_iter_t *iter) {
    while (1) {
        topo_entry_t *entry = &(iter->topo->table[iter->offs]);
        if (entry->src == entry->dst) return NULL;
        iter->offs = (iter->offs + 1) % TOPO_TABLESIZE;
        if (entry->src == iter->src) return entry;
    }
}

////////////////////////////////////////////////////////////////////////////////

void topo_iter_free(topo_iter_t *iter) {
    free(iter);
}
