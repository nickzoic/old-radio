// $Id: node.h,v 1.4 2009-10-07 18:50:39 nick Exp $

#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

#include "vtime.h"
#include "beacon.h"
#include "neigh.h"

#define NODE_STATUS_ASLEEP (0)
#define NODE_STATUS_WAKING (1)
#define NODE_STATUS_AWAKE (2)
#define NODE_STATUS_ROOT (9)


typedef struct node_s {
    
    int id;
    int status;
    neigh_tab_t ntab;
    
} node_t;

node_t *node_new(int id);
void node_event(node_t *node, vtime_t vtime, beacon_t *beac);
void node_destroy(node_t *node);

#endif
