// $Id: node.c,v 1.26 2009-10-21 22:55:27 nick Exp $

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
    node->status = NODE_STATUS_ASLEEP;
    node->neigh_table = neigh_table_new();
    virtloc_init(&node->virtloc, id);
}

//////////////////////////////////////////////////////////////////////  node_new

node_t *node_new(node_id_t id) {
    node_t *node = calloc(1,sizeof(node_t));
    node_init(node, id);
    return node;
}

///////////////////////////////////////////////////////////////  node_set_status

void node_set_status(node_t *node, vtime_t vtime, int status) {
    printf(VTIME_FORMAT " %6d Status %d %d\n", vtime, node->id, node->status, status);
    node->status = status;
}

////////////////////////////////////////////////////////  node_register_callback

void node_register_callback(void (*callback)(node_t *, vtime_t, packet_t *)) {
    Node_callback = callback;
}

///////////////////////////////////////////////////////////////////  node_beacon

packet_t *node_beacon(node_t *node, vtime_t vtime) {
    char buffer[NODE_BEACON_SIZE];
    node_beacon_t *beacon = (node_beacon_t *)buffer;
    
    beacon->header.packet_type = PACKET_TYPE_BEACON;
    beacon->header.status = node->status;
    beacon->neigh[0].stratum = 0;
    beacon->neigh[0].loc = node->virtloc.loc;
    beacon->neigh[0].id = node->id;

    printf(VTIME_FORMAT " %6d Beacon %d %d %d %d",
            vtime, node->id, node->status,
            node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
    
    int nneigh = 1;
    int maxneigh = (NODE_BEACON_SIZE - 2) / sizeof(neigh_t);
    neigh_iter_t *iter = neigh_iter_new(node->neigh_table);
    while (nneigh < maxneigh) {
        neigh_t *n = neigh_iter_next(iter);
        if (!n) break;
        if (n->stratum == 1) {
            printf(" %d", n->id);
            beacon->neigh[nneigh] = *n;
            nneigh++;
        }
    }
    neigh_iter_free(iter);
    printf("\n");
    
    return packet_new(sizeof(node_beacon_header_t) + nneigh * sizeof(neigh_t), buffer);
}

///////////////////////////////////////////////////////////  node_receive_beacon

void node_receive_beacon(node_t *node, vtime_t vtime, node_beacon_t *beacon, int nneigh) {
    printf(VTIME_FORMAT " %6d RecvBeacon %d %d %d\n", vtime, node->id,
           beacon->neigh[0].id, beacon->header.status, nneigh);
    int seen_self = 0;        
    if (node->status == NODE_STATUS_ASLEEP) {
        if (beacon->header.status == NODE_STATUS_WAKING) return;
        node_set_status(node, vtime, NODE_STATUS_WAKING);
        node->virtloc.loc = beacon->neigh[0].loc;
        loc_perturb(&node->virtloc.loc, 5);
        printf(VTIME_FORMAT " %6d Wake %d %d %d\n", vtime, node->id,
               node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
        Node_callback(node, vtime + NODE_BEACON_PERIOD / 2, NULL);
    } else if (node->status == NODE_STATUS_WAKING) {
        node_set_status(node, vtime, NODE_STATUS_AWAKE);
    }
    
    for (int i=0; i<nneigh; i++) {
        if (beacon->neigh[i].id == node->id) {
            seen_self = 1;
            continue;
        }
        neigh_t nnn = beacon->neigh[i];
        nnn.stratum++;
        printf(VTIME_FORMAT " %6d Neigh %d %d (%d %d %d) %g\n",
               vtime, node->id, nnn.id, nnn.stratum,
               nnn.loc.x, nnn.loc.y, nnn.loc.z,
               loc_dist(&node->virtloc.loc, &nnn.loc)
        );
        neigh_table_insert(node->neigh_table, nnn, vtime);
    }
    if (node->status == NODE_STATUS_AWAKE) {
        //neigh_table_cull(node->neigh_table, vtime);
        //virtloc_recalc(&node->virtloc, node->neigh_table);
    }
}

//////////////////////////////////////////////////////////////////  node_receive

void node_receive(node_t *node, vtime_t vtime, packet_t *packet) {
    assert(node);
    assert(packet);
    assert(Node_callback);
    
    switch (packet->data[0]) {
        
        case PACKET_TYPE_BEACON:
            do { } while (0);
            node_beacon_t *beacon = (node_beacon_t *)packet->data;
            assert(((packet->length - sizeof(node_beacon_header_t)) % sizeof(neigh_t)) == 0);
            int nneigh = (packet->length - sizeof(node_beacon_header_t)) / sizeof(neigh_t);
            node_receive_beacon(node, vtime, beacon, nneigh);
          break;
        
        case PACKET_TYPE_FLOOD:
            if (vtime > node->flood_timeout) {
                printf(VTIME_FORMAT " %6d Flood %.*s\n", vtime, node->id, (int)(packet->length)-1, (packet->data)+1);
                Node_callback(node, vtime, packet);
                node->flood_timeout = vtime + NODE_FLOOD_TIMEOUT;
            }
          break;
        
        default:
            printf(VTIME_FORMAT " %6d ERR Unknown packet type %02X length %ld\n",
                   vtime, node->id, (packet->data)[0], packet->length);
          break;
    }
    
}

////////////////////////////////////////////////////////////////////  node_timer

void node_timer(node_t *node, vtime_t vtime) {
    assert(node);
    assert(Node_callback);
    
    printf(VTIME_FORMAT " %6d Time\n", vtime, node->id);

    if (node->status == NODE_STATUS_AWAKE) {
        printf(VTIME_FORMAT " %6d Virt1 %d %d %d\n", vtime, node->id,
               node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
        virtloc_recalc(&node->virtloc, node->neigh_table);
        printf(VTIME_FORMAT " %6d Virt2 %d %d %d\n", vtime, node->id,
               node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
    }
    
    packet_t *p = node_beacon(node, vtime);
    
    Node_callback(node, vtime, p);
    long delay = NODE_BEACON_PERIOD * ((float)rand() * 2 / RAND_MAX + 0.5);
    Node_callback(node, vtime + delay, NULL);

    packet_free(p);
}

///////////////////////////////////////////////////////////////////  node_deinit

void node_deinit(node_t *node) {
    neigh_table_free(node->neigh_table);    
}

/////////////////////////////////////////////////////////////////////  node_free

void node_free(node_t *node) {
    node_deinit(node);
    free(node);
}

////////////////////////////////////////////////////////////////////////////////