// $Id: node.c,v 1.5 2009-10-07 22:06:15 nick Exp $

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

void node_register_sender(node_t *node, void (*sender_func)(node_t *, packet_t *)) {
    node->sender_func = sender_func;
}

void node_register_timer(node_t *node, void (*timer_func)(node_t *, vtime_t)) {
    node->timer_func = timer_func;
}

void node_receive(node_t *node, vtime_t vtime, packet_t *packet) {
    
}

void node_timer(node_t *node, vtime_t vtime) {
    
}

void node_free(node_t *node) {
    free(node);
}