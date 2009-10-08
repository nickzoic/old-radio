// $Id: node.h,v 1.8 2009-10-08 00:01:44 nick Exp $

#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

#include "loc.h"
#include "vtime.h"
#include "packet.h"
#include "neigh.h"

#define NODE_STATUS_ASLEEP (0)
#define NODE_STATUS_WAKING (1)
#define NODE_STATUS_AWAKE (2)
#define NODE_STATUS_ROOT (9)

#define NODE_MAX_NEIGH (30)

typedef struct node_s {
    
    int id;
    int status;
    
    neigh_table_t neigh_table;
    
    vtime_t flood_timeout;
    
    void (*sender)(struct node_s *, packet_t *);
    void (*timer)(struct node_s *, vtime_t);
    
} node_t;

node_t *node_new(int id);
void node_set_status(node_t *node, vtime_t vtime, int status);

void node_register_sender(node_t *node, void (*sender)(node_t *, packet_t *));
void node_register_timer(node_t *node, void (*timer)(node_t *, vtime_t));

void node_receive(node_t *node, vtime_t vtime, packet_t *packet);
void node_timer(node_t *node, vtime_t vtime);

void node_free(node_t *node);

#endif
