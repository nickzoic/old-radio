// $Id: node.c,v 1.11 2009-10-14 07:08:00 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "node.h"

#define NODE_FLOOD_TIMEOUT_MS (1000)
#define NODE_BEACON_PERIOD_MS (1000)

// this callback hook lets different frontends call the same node class
// without too much pain.

void (*Node_callback)(node_t *, vtime_t, packet_t *) = NULL;

// PUBLIC

void node_init(node_t *node, node_id_t id) {
    node->id = id;
}

node_t *node_new(node_id_t id) {
    node_t *node = calloc(1,sizeof(node_t));
    node_init(node, id);
    return node;
}

void node_set_status(node_t *node, vtime_t vtime, int status) {
    node->status = status;
    printf(VTIME_FORMAT " %6d S %d\n", vtime, node->id, status);
}

void node_register_callback(void (*callback)(node_t *, vtime_t, packet_t *)) {
    Node_callback = callback;
}

void node_receive(node_t *node, vtime_t vtime, packet_t *packet) {
    assert(node);
    assert(packet);
    
    switch (packet->data[0]) {
        
        case PACKET_TYPE_BEACON:
            if (node->status == NODE_STATUS_ASLEEP)
                node->status = NODE_STATUS_WAKING;
            neigh_t *nn = (neigh_t *)(packet->data+1);
            int nneigh = (packet->length - 1) / sizeof(neigh_t);
            assert( (packet->length - 1) % sizeof(neigh_t) == 0 );
            for (int i=0; i<nneigh; i++) {
                printf(VTIME_FORMAT " %6d N %d %d %d %d %d\n",
                       vtime, node->id, nn[i].id, nn[i].stratum, nn[i].loc.x, nn[i].loc.y, nn[i].loc.z);
                neigh_table_insert(&(node->neigh_table), nn[i], vtime);
            }
          break;
        
        case PACKET_TYPE_FLOOD:
            assert(Node_callback);
            
            if (vtime > node->flood_timeout) {
                printf(VTIME_FORMAT " %6d F %.*s\n", vtime, node->id, (int)(packet->length)-1, (packet->data)+1);
                Node_callback(node, vtime, packet);
                node->flood_timeout = vtime_add_ms(vtime, NODE_FLOOD_TIMEOUT_MS);
            }
          break;
        
        default:
            printf(VTIME_FORMAT " %6d W Unknown packet type %02X length %d",
                   vtime, node->id, packet->data[0], (int)packet->length);
          break;
    }
}

void node_timer(node_t *node, vtime_t vtime) {
    assert(node);
    assert(Node_callback);
    
    printf(VTIME_FORMAT " %6d T\n", vtime, node->id);
    
    char s[200];
    sprintf(s, "\xF0hello" VTIME_FORMAT "!", vtime);
    
    packet_t *p = packet_new(sizeof(s), s);
    
    Node_callback(node, vtime, p);
    Node_callback(node, vtime_add_ms(vtime, NODE_BEACON_PERIOD_MS), NULL);

    packet_free(p);
}

void node_free(node_t *node) {
    free(node);
}