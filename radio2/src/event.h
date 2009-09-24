// $Id: event.h,v 1.2 2009-09-24 10:43:24 nick Exp $

#ifndef _EVENT_H
#define _EVENT_H

#include "vtime.h"
#include "beacon.h"
#include "node.h"

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

#endif