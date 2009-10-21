// $Id: node.h,v 1.15 2009-10-21 07:10:26 nick Exp $

#ifndef _NODE_H
#define _NODE_H

#include <stdint.h>

#include "loc.h"
#include "vtime.h"
#include "packet.h"
#include "neigh.h"
#include "virtloc.h"

#define NODE_STATUS_ASLEEP (0)
#define NODE_STATUS_WAKING (1)
#define NODE_STATUS_AWAKE (2)
#define NODE_STATUS_ROOT (9)

#define NODE_MAX_NEIGH (30)

typedef neigh_id_t node_id_t;
typedef neigh_stratum_t node_stratum_t;

typedef struct node_s {
    
    node_id_t id;
    unsigned char status;
    
    neigh_table_t *neigh_table;
    loc_t loc;
    
    virtloc_t virtloc;
    
    vtime_t flood_timeout;
    vtime_t wake_time;
    
    void (*callback)(struct node_s *, vtime_t, packet_t *);
    
} node_t;

typedef struct node_beacon_header_s {
    unsigned char packet_type;
    unsigned char status;
} node_beacon_header_t;

typedef struct node_beacon_s {
    node_beacon_header_t header;        
    neigh_t neigh[];
} node_beacon_t;

void node_init(node_t *node, node_id_t id);
node_t *node_new(node_id_t id);
void node_set_status(node_t *node, vtime_t vtime, int status);

void node_register_callback(void (*sender)(node_t *, vtime_t, packet_t *));

void node_receive(node_t *node, vtime_t vtime, packet_t *packet);
void node_timer(node_t *node, vtime_t vtime);

void node_deinit(node_t *node);
void node_free(node_t *node);

#endif
