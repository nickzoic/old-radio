// $Id: queue.h,v 1.4 2009-10-14 05:31:17 nick Exp $

#ifndef _QUEUE_H
#define _QUEUE_H

#include "vtime.h"
#include "node.h"
#include "packet.h"


typedef struct queue_event_s {
    vtime_t vtime;
    node_t *node;
    packet_t *packet;
} queue_event_t;

#define QUEUE_EVENT_NULL { VTIME_INF, NULL, NULL }

typedef struct queue_s {
    long size;
    long nevents;
    vtime_t eschaton;
    
    queue_event_t *events;
} queue_t;


queue_t *queue_new();
void queue_set_eschaton(queue_t *queue, vtime_t eschaton);

void queue_insert(queue_t *queue, queue_event_t event);
queue_event_t queue_pop(queue_t *queue);

void queue_free(queue_t *);

#endif
