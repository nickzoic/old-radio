// $Id: node.c,v 1.20 2009-10-19 01:02:13 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "node.h"

#define NODE_FLOOD_TIMEOUT (1 * VTIME_SECONDS)
#define NODE_BEACON_PERIOD (1 * VTIME_SECONDS)

#define NODE_BEACON_SIZE (200)

// this callback hook lets different frontends call the same node class
// without too much pain.

void (*Node_callback)(node_t *, vtime_t, packet_t *) = NULL;

////////////////////////////////////////////////////////////////////////////////

void node_init(node_t *node, node_id_t id) {
    node->id = id;
    neigh_table_init(node->neigh_table);
    virtloc_init(&node->virtloc, id);
}

////////////////////////////////////////////////////////////////////////////////

node_t *node_new(node_id_t id) {
    node_t *node = calloc(1,sizeof(node_t));
    node_init(node, id);
    return node;
}

////////////////////////////////////////////////////////////////////////////////

void node_set_status(node_t *node, vtime_t vtime, int status) {
    node->status = status;
    printf(VTIME_FORMAT " %6d S %d\n", vtime, node->id, status);
}

////////////////////////////////////////////////////////////////////////////////

void node_register_callback(void (*callback)(node_t *, vtime_t, packet_t *)) {
    Node_callback = callback;
}

////////////////////////////////////////////////////////////////////////////////

void node_receive(node_t *node, vtime_t vtime, packet_t *packet) {
    assert(node);
    assert(packet);
    
    switch (packet->data[0]) {
        
        case PACKET_TYPE_BEACON:
            if (node->status == NODE_STATUS_ASLEEP) {
                node->status = NODE_STATUS_WAKING;
                Node_callback(node, vtime + NODE_BEACON_PERIOD, NULL);            
            }
            neigh_t *nn = (neigh_t *)(packet->data+1);
            int nneigh = (packet->length - 1) / sizeof(neigh_t);
            assert( (packet->length - 1) % sizeof(neigh_t) == 0 );
            
            neigh_table_cull(node->neigh_table, vtime);
            for (int i=0; i<nneigh; i++) {
                neigh_t nnn = nn[i];
                printf(VTIME_FORMAT " %6d N %d s%d (%d %d %d)\n",
                       vtime, node->id, nnn.id, nnn.stratum, nnn.loc.x, nnn.loc.y, nnn.loc.z);
                nnn.stratum++;
                neigh_table_insert(node->neigh_table, nnn, vtime);
            }
            if (node->id) {
                virtloc_recalc(&node->virtloc, node->neigh_table);
            }
          break;
        
        case PACKET_TYPE_FLOOD:
            assert(Node_callback);
            
            if (vtime > node->flood_timeout) {
                printf(VTIME_FORMAT " %6d F %.*s\n", vtime, node->id, (int)(packet->length)-1, (packet->data)+1);
                Node_callback(node, vtime, packet);
                node->flood_timeout = vtime + NODE_FLOOD_TIMEOUT;
            }
          break;
        
        default:
            printf(VTIME_FORMAT " %6d W Unknown packet type %02X length %ld\n",
                   vtime, node->id, (packet->data)[0], packet->length);
          break;
    }
    
}

////////////////////////////////////////////////////////////////////////////////

packet_t *node_beacon(node_t *node) {
    char buffer[NODE_BEACON_SIZE];
    buffer[0] = PACKET_TYPE_BEACON;
    neigh_t *np = (neigh_t *)(buffer + 1);
    np[0].stratum = 0;
    np[0].loc = node->virtloc.loc;
    np[0].id = node->id;

    int nneigh = 1;
    int maxneigh = (NODE_BEACON_SIZE - 1) / sizeof(neigh_t);
    neigh_iter_t *iter = neigh_iter_new(node->neigh_table);
    while (nneigh < maxneigh) {
        neigh_t *n = neigh_iter_next(iter);
        if (!n) break;
        if (n->stratum == 1) {
            np[nneigh] = *n;
            nneigh++;
        }
    }
    
    return packet_new(1 + nneigh * sizeof(neigh_t), buffer);
}

////////////////////////////////////////////////////////////////////////////////

void node_timer(node_t *node, vtime_t vtime) {
    assert(node);
    assert(Node_callback);
    
    printf(VTIME_FORMAT " %6d T\n", vtime, node->id);

    if (node->id) virtloc_recalc(&node->virtloc, node->neigh_table);
    
    packet_t *p = node_beacon(node);
    
    printf(VTIME_FORMAT " %6d B (%d %d %d) %ld\n",
            vtime, node->id, node->virtloc.loc.x, node->virtloc.loc.y,
            node->virtloc.loc.z, p->length / sizeof(neigh_t));
    
    Node_callback(node, vtime, p);
    Node_callback(node, vtime + NODE_BEACON_PERIOD, NULL);

    packet_free(p);
}

////////////////////////////////////////////////////////////////////////////////

void node_free(node_t *node) {
    free(node);
}

////////////////////////////////////////////////////////////////////////////////