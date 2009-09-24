// $Id: node.h,v 1.1 2009-09-24 00:21:16 nick Exp $

typedef struct node_s {
    
    int id;
    neigh_tab_t ntab;
    
} node_t;

node_t *node_new(int id);
void node_event(node_t *node, vtime_t vtime, beacon_t *beac);
void node_destroy(node_t *node);

