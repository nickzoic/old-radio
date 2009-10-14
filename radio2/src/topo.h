// $Id: topo.h,v 1.1 2009-10-14 05:31:17 nick Exp $

#ifndef _TOPO_H
#define _TOPO_H

#include "node.h"

typedef struct topo_entry_s {
    node_id_t src;
    node_id_t dst;
    float prob;
} topo_entry_t;

typedef struct topo_s {
    topo_entry_t *table;
    size_t tablesize;
    size_t tableoffs;
    size_t entries;
    node_id_t max_id;
} topo_t;

typedef struct topo_iter_s {
    topo_t *topo;
    size_t offs;
    node_id_t src;
} topo_iter_t;

topo_t *topo_new();

void topo_file_read(topo_t *topo, FILE *fp);

node_id_t topo_max_id(topo_t *topo);

void topo_free(topo_t *topo);

topo_iter_t *topo_iter_new(topo_t *topo, node_id_t src);

topo_entry_t *topo_iter_next(topo_iter_t *iter);

void topo_iter_free(topo_iter_t *iter);


#endif