// $Id: node.c,v 1.2 2009-09-24 10:43:25 nick Exp $

#include <stdlib.h>

#include "node.h"

node_t *node_new(int id) {
    node_t *n = calloc(1,sizeof(node_t));
    n->id = id;
    return n;
}

void node_event(node_t *node, vtime_t vtime, beacon_t *beacon) {
    if (beacon) {
        // beacon received event
        
    } else {
        // timer event
        
    }
}

void node_destroy(node_t *node) {
    free(node);
}