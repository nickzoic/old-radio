// $Id: node.c,v 1.32 2010-01-31 00:02:21 nick Exp 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"

#define NODE_FLOOD_TIMEOUT (1 * VTIME_SECONDS)
#define NODE_BEACON_PERIOD (1 * VTIME_SECONDS)
#define NODE_WAKEUP_PERIOD (2.5 * VTIME_SECONDS)

#define NODE_BEACON_SIZE (200)
#define NODE_BEACON_MAXSTRAT (2)

#define NODE_TTL_MAX (100)

char *NODE_STATUS_STRINGS[] = { "ASLEEP", "WAKING", "AWAKE", "ROOT" };

// this callback hook lets different frontends call the same node class
// without too much pain.

void (*Node_callback)(node_t *, vtime_t, packet_t *, void *) = NULL;

////////////////////////////////////////////////////////////////////////////////

void node_init(node_t *node, node_id_t id, char *node_opts) {
    node->id = id;
    node->status = NODE_STATUS_ASLEEP;
    node->neigh_table = neigh_table_new();
    virtloc_init(&node->virtloc, id);

    if (strchr(node_opts, 'R') && (id == 0)) {
	node_set_status(node, VTIME_ZERO, NODE_STATUS_ROOT);
    } else if (strchr(node_opts, 'A')) {
	node_set_status(node, VTIME_ZERO, NODE_STATUS_AWAKE);
    }
}

//////////////////////////////////////////////////////////////////////  node_new

node_t *node_new(node_id_t id, char *node_opts) {
    node_t *node = calloc(1,sizeof(node_t));
    node_init(node, id, node_opts);
    return node;
}

///////////////////////////////////////////////////////////////  node_set_status

void node_set_status(node_t *node, vtime_t vtime, int status) {
    printf(VTIME_FORMAT " %6d Status %d:%s %d:%s\n", vtime, node->id,
            node->status, NODE_STATUS_STRINGS[node->status], status, NODE_STATUS_STRINGS[status]);
    node->status = status;
    if (status != NODE_STATUS_ASLEEP && Node_callback) {
    	long delay = 0;
	if (status != NODE_STATUS_ROOT) {
		delay += NODE_BEACON_PERIOD * ((float)rand() / RAND_MAX + 0.5);
	}
    	Node_callback(node, vtime + delay, NULL, NULL);
    }
}

////////////////////////////////////////////////////////  node_register_callback

void node_register_callback(void (*callback)(node_t *, vtime_t, packet_t *, void *)) {
    Node_callback = callback;
}

////////////////////////////////////////////////////////////////  node_route_mfr

node_id_t node_route_mfr(node_t *node, node_id_t dest_id, loc_t dest_loc, int maxstrat) {

    unsigned long min_dist2 = loc_dist2(&node->virtloc.loc, &dest_loc);
    neigh_t *min_neigh = NULL;

    neigh_iter_t *iter = neigh_iter_new(node->neigh_table);
    neigh_t *n;

    while ((n = neigh_iter_next(iter))) {
	if (n->stratum <= maxstrat) {
	    if (n->id == dest_id) {
		min_neigh = n;
		break;
	    }
	    unsigned long dist2 = loc_dist2(&n->loc, &dest_loc);
	    if (dist2 < min_dist2) {
		min_dist2 = dist2;
		min_neigh = n;
	    }
	}
    }

    if (min_neigh) {
	if (min_neigh->stratum == 1) return min_neigh->id;
	return node_route_mfr(node, min_neigh->id, min_neigh->loc, min_neigh->stratum - 1);
    }

    return NODE_ID_INVALID;
    
}

///////////////////////////////////////////////////////////////  node_neigh_dump

void node_neigh_dump(node_t *node, vtime_t vtime) {
    neigh_iter_t *iter = neigh_iter_new(node->neigh_table);
    neigh_t *n;
    while ((n = neigh_iter_next(iter))) {
        
        printf(VTIME_FORMAT " %6d NeighDump %d %d  %d %d %d\n",
            vtime, node->id, n->id, n->stratum, 
            n->loc.x, n->loc.y, n->loc.z);
    }
    
    neigh_iter_free(iter);
    
}

///////////////////////////////////////////////////////////////////  node_beacon

packet_t *node_beacon(node_t *node, vtime_t vtime) {
    char buffer[NODE_BEACON_SIZE];
    node_beacon_t *beacon = (node_beacon_t *)buffer;
    
    node_neigh_dump(node,vtime);
        
    beacon->header.packet_type = PACKET_TYPE_BEACON;
    beacon->header.status = node->status;
    beacon->neigh[0].stratum = 0;
    beacon->neigh[0].loc = node->virtloc.loc;
    beacon->neigh[0].id = node->id;

    printf(VTIME_FORMAT " %6d Beacon %d  %d %d %d ",
            vtime, node->id, node->status,
            node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
    
    int nneigh = 1;
    int maxneigh = (NODE_BEACON_SIZE - 2) / sizeof(neigh_t);
    neigh_iter_t *iter = neigh_iter_new(node->neigh_table);
    while (nneigh < maxneigh) {
        neigh_t *n = neigh_iter_next(iter);
        if (!n) break;
        if ((n->stratum >= 1 && n->stratum <= NODE_BEACON_MAXSTRAT) || n->stratum == NEIGH_STRATUM_INF) {
            printf("  %d %d", n->id, n->stratum);
            beacon->neigh[nneigh] = *n;
            nneigh++;
        }
    }
    neigh_iter_free(iter);
    printf("\n");
    
    return packet_new(sizeof(node_beacon_header_t) + nneigh * sizeof(neigh_t), buffer);
}

///////////////////////////////////////////////////////////  node_receive_beacon

void node_receive_beacon(node_t *node, vtime_t vtime, packet_t *packet) {
    node_beacon_t *beacon = (node_beacon_t *)packet->data;
    assert(((packet->length - sizeof(node_beacon_header_t)) % sizeof(neigh_t)) == 0);
    int nneigh = (packet->length - sizeof(node_beacon_header_t)) / sizeof(neigh_t);
                
    printf(VTIME_FORMAT " %6d RecvBeacon %d %d %d\n", vtime, node->id,
           beacon->neigh[0].id, beacon->header.status, nneigh);
    
    if (node->status == NODE_STATUS_ASLEEP) {
    
        // If this is a beacon from a waking node, it's not enough to
        // wake us up ...
        
        if (beacon->header.status == NODE_STATUS_WAKING) return;
        
        // ... otherwise, if we were asleep, this beacon wakes us up ...
        
        node_set_status(node, vtime, NODE_STATUS_WAKING);
        node->wake_time = vtime + NODE_WAKEUP_PERIOD;
        
        // ... and gives us a default vloc nearby ...
        
        node->virtloc.loc = beacon->neigh[0].loc;
        loc_perturb(&node->virtloc.loc, 5);
        
        printf(VTIME_FORMAT " %6d Waking  %d %d %d\n", vtime, node->id,
               node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
        
        // ... and starts the callback timer.
        
        Node_callback(node, vtime + NODE_BEACON_PERIOD, NULL, NULL);
    }
        
    // Check that this neighbour is bidirectional before listing them ...
    
    int seen_self = 0;
    for (int i=0; i<nneigh; i++) {
        if (beacon->neigh[i].id == node->id && (
                beacon->neigh[i].stratum == 1 || beacon->neigh[i].stratum == NEIGH_STRATUM_INF)
        ) {
            seen_self = 1;
        }
    }
    
    if (!seen_self) {
        neigh_t nnn = beacon->neigh[0];
        nnn.stratum = NEIGH_STRATUM_INF;
        neigh_table_insert(node->neigh_table, nnn, vtime);
        
        printf(VTIME_FORMAT " %6d NoSeeSelf\n", vtime, node->id);
        node_neigh_dump(node, vtime);
        return;
    }
    
    // Record information in neighbour table
    
    for (int i=0; i<nneigh; i++) {
        if (beacon->neigh[i].id != node->id && beacon->neigh[i].stratum != NEIGH_STRATUM_INF) {
            neigh_t nnn = beacon->neigh[i];
            nnn.stratum++;
            
            printf(VTIME_FORMAT " %6d RecvBeaconNeigh %d %d  %d %d %d  %g\n",
                   vtime, node->id, nnn.id, nnn.stratum,
                   nnn.loc.x, nnn.loc.y, nnn.loc.z,
                   loc_dist(&node->virtloc.loc, &nnn.loc)
            );
            neigh_table_insert(node->neigh_table, nnn, vtime);
       }
    }
    
    //neigh_table_cull(node->neigh_table, vtime);
    
    //if (node->status != NODE_STATUS_ROOT) {
    //    virtloc_recalc(&node->virtloc, node->neigh_table);
    //}
    node_neigh_dump(node, vtime);
}

////////////////////////////////////////////////////////////////  node_send_data

void node_send_data(node_t *node, vtime_t vtime, packet_data_t data)
{
    packet_t *packet = packet_new(sizeof(data), &data);
    Node_callback(node, vtime, packet, NULL);
    packet_free(packet);
}

/////////////////////////////////////////////////////////////  node_forward_data

void node_forward_data(node_t *node, vtime_t vtime, packet_data_t data)
{
    data.ttl--;
    data.hop = node_route_mfr(node, data.dst, data.dstloc, 1);
    
    printf(VTIME_FORMAT " %6d FwdData %6d %6d %6d ttl %d\n",
           vtime, node->id, data.src, data.hop, data.dst, data.ttl);
    
    if (data.hop == NODE_ID_INVALID) return;
    
    node_send_data(node, vtime, data);
}

/////////////////////////////////////////////////////////////  node_receive_data

void node_receive_data(node_t *node, vtime_t vtime, packet_t *packet)
{
    assert(packet->length >= sizeof(packet_data_t));
    packet_data_t *data = (packet_data_t *)packet->data;
    if (data->hop != node->id) return;
    
    printf(VTIME_FORMAT " %6d RecvData %6d -> %6d ttl %d\n",
           vtime, node->id, data->src, data->dst, data->ttl);
    
    if (data->dst == node->id) {
        printf(VTIME_FORMAT " %6d Received %6d " VTIME_FORMAT "\n",
               vtime, node->id, data->src, data->vtime);
    } else {
        node_forward_data(node, vtime, *data);
    }
}

///////////////////////////////////////////////////////////////  node_send_flood

void node_send_flood(node_t *node, vtime_t vtime, packet_t *packet)
{
    if (vtime > node->flood_timeout) {
        printf(VTIME_FORMAT " %6d SendFlood\n", vtime, node->id);
        Node_callback(node, vtime, packet, NULL);
        node->flood_timeout = vtime + NODE_FLOOD_TIMEOUT;
    }
}

//////////////////////////////////////////////////////////////  node_start_flood

void node_start_flood(node_t *node, vtime_t vtime)
{
    packet_flood_t *
}
////////////////////////////////////////////////////////////  node_receive_flood

void node_receive_flood(node_t *node, vtime_t vtime, packet_t *packet)
{
    printf(VTIME_FORMAT " %6d RecvFlood\n", vtime, node->id);
    node_send_flood(node, vtime, packet);
    
    packet_data_t reply;
    reply.src = node->id;
    reply.srcloc = node->virtloc.loc;
    
    node_send_data(node, vtime, reply)
}

//////////////////////////////////////////////////////////////////  node_receive

void node_receive(node_t *node, vtime_t vtime, packet_t *packet) {
    assert(node);
    assert(packet);
    assert(Node_callback);
    
    unsigned char data_type = packet->length ? packet->data[0] : 0;
    
    if (data_type == PACKET_TYPE_BEACON) {
        node_receive_beacon(node, vtime, packet);
    }
    else if (data_type == PACKET_TYPE_FLOOD) {
        node_receive_flood(node, vtime, packet);
    }    
    else if (data_type == PACKET_TYPE_DATA) {
        node_receive_data(node, vtime, packet);
    }    
    else {
        printf(VTIME_FORMAT " %6d ERR Unknown packet type %02X length %ld\n",
                vtime, node->id, data_type, (long)packet->length);
    }
}

///////////////////////////////////////////////////////////  node_virtloc_recalc
    
void node_virtloc_recalc(node_t *node, vtime_t vtime) {
    printf(VTIME_FORMAT " %6d VirtBefore  %d %d %d\n", vtime, node->id,
           node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
    virtloc_recalc(&node->virtloc, node->neigh_table);
    printf(VTIME_FORMAT " %6d VirtAfter  %d %d %d\n", vtime, node->id,
           node->virtloc.loc.x, node->virtloc.loc.y, node->virtloc.loc.z);
}

////////////////////////////////////////////////////////////////////  node_timer

void node_timer(node_t *node, vtime_t vtime, void *extra) {
    assert(node);
    assert(Node_callback);
    
    printf(VTIME_FORMAT " %6d Time\n", vtime, node->id);

    //neigh_table_cull(node->neigh_table, vtime);
    
    if (node->status == NODE_STATUS_WAKING && vtime >= node->wake_time) {
        node_set_status(node, vtime, NODE_STATUS_AWAKE);
    }
    
    node_virtloc_recalc(node, vtime);

    // send a beacon
    packet_t *p = node_beacon(node, vtime);
    Node_callback(node, vtime, p, NULL);
    packet_free(p);
    
    // set timer for next event
    long delay = NODE_BEACON_PERIOD * ((float)rand() / RAND_MAX + 0.5);
    Node_callback(node, vtime + delay, NULL, extra);
}

///////////////////////////////////////////////////////////////  node_route_test

void node_route_test(node_t *node, vtime_t vtime) {
    // send a data packet to the root node to test routing

    packet_data_t data;
    data.packet_type = PACKET_TYPE_DATA;
    data.src = node->id;
    data.srcloc = node->virtloc.loc;
    data.dst = 0;
    loc_zero(&data.dstloc);
    data.ttl = NODE_TTL_MAX;
    data.vtime = vtime;
    
    printf(VTIME_FORMAT " %6d NodeRouteTest\n", vtime, node->id);
    
    node_forward_data(node, vtime, data);
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
