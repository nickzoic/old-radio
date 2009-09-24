// $Id: node.h,v 1.3 2009-09-24 10:43:25 nick Exp $

#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

#include "vtime.h"
#include "beacon.h"
#include "neigh.h"

typedef struct node_s {
    
    int id;
    neigh_tab_t ntab;
    
} node_t;

node_t *node_new(int id);
void node_event(node_t *node, vtime_t vtime, beacon_t *beac);
void node_destroy(node_t *node);

#endif
