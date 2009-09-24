// $Id: node.c,v 1.1 2009-09-24 00:21:16 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "loc.h"
#include "vtime.h"
#include "beacon.h"
#include "neigh.h"
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