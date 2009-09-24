// $Id: event.h,v 1.1 2009-09-24 00:21:14 nick Exp $

typedef struct event_s {
    vtime_t vtime;
    node_t *node;
    beacon_t *beacon;
} event_t;

void event_init();

void event_insert(event_t event);
event_t event_pop();
void event_heap_print();

void event_destroy();
