// $Id: node.h,v 1.5 2009-10-07 21:46:43 nick Exp $

#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

#include "vtime.h"
#include "packet.h"
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
void node_set_status(node_t *node, int status);

void node_register_sender(node_t *node, void (*sender)(node_t *, packet_t *));
void node_register_timer(node_t *node, void (*timer)(node_t *, vtime_t));

void node_receive(node_t *node, vtime_t vtime, packet_t *packet);
void node_timer(node_t *node, vtime_t vtime);

void node_free(node_t *node);

#endif
