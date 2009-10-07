// $Id: node.c,v 1.4 2009-10-07 21:46:43 nick Exp $

#include <stdlib.h>

#include "node.h"

node_t *node_new(int id) {
    node_t *n = calloc(1,sizeof(node_t));
    n->id = id;
    return n;
}

void node_set_status(node_t *node, vtime_t vtime, int status) {
    node->status = status;
    printf(VTIME_FORMAT " %d S %d\n", vtime, node->id, status);
}

void node_event(node_t *node, vtime_t vtime, beacon_t *beacon) {
    if (beacon) {
        // beacon received event
        
    } else {
        node_recalc();
        
        // timer event
        
    }
}

void node_free(node_t *node) {
    free(node);
}