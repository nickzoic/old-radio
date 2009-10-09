// $Id: node.c,v 1.9 2009-10-09 10:00:13 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "node.h"

#define NODE_FLOOD_TIMEOUT_MS (1000)
#define NODE_BEACON_PERIOD_MS (1000)

// PUBLIC

node_t *node_new(node_id_t id) {
    node_t *n = calloc(1,sizeof(node_t));
    n->id = id;
    return n;
}

void node_set_status(node_t *node, vtime_t vtime, int status) {
    node->status = status;
    printf(VTIME_FORMAT " %d S %d\n", vtime, node->id, status);
}

void node_register_sender(node_t *node, void (*sender)(node_t *, packet_t *)) {
    node->sender = sender;
}

void node_register_timer(node_t *node, void (*timer)(node_t *, vtime_t)) {
    node->timer = timer;
}

void node_receive(node_t *node, vtime_t vtime, packet_t *packet) {
    switch (packet->data[0]) {
        case PACKET_TYPE_BEACON:
            if (node->status == NODE_STATUS_ASLEEP)
                node->status = NODE_STATUS_WAKING;
            neigh_t *nn = (neigh_t *)(packet->data+1);
            int nneigh = (packet->length - 1) / sizeof(neigh_t);
            assert( (packet->length - 1) % sizeof(neigh_t) == 0 );
            for (int i=0; i<nneigh; i++) {
                printf(VTIME_FORMAT " %d N %d %d %d %d %d\n",
                       vtime, node->id, nn[i].id, nn[i].stratum, nn[i].loc.x, nn[i].loc.y, nn[i].loc.z);
                neigh_table_insert(&(node->neigh_table), nn[i], vtime);
            }
          break;
        case PACKET_TYPE_FLOOD:
            if (vtime > node->flood_timeout) {
                printf(VTIME_FORMAT " %d F %.*s\n", vtime, node->id, (packet->length)-1, (packet->data)+1);
                node->sender(node, packet);
                node->flood_timeout = vtime_add_ms(vtime, NODE_FLOOD_TIMEOUT_MS);
            }
          break;
        default:
            printf(VTIME_FORMAT " %d W Unknown packet type %02X length %d",
                   vtime, node->id, packet->data[0], packet->length);
          break;
    }
}

void node_timer(node_t *node, vtime_t vtime) {
    packet_t *p = packet_new(6, "hello");
    
    node->sender(node, p);
    node->timer(node, vtime_add_ms(vtime, NODE_BEACON_PERIOD_MS));
}

void node_free(node_t *node) {
    free(node);
}