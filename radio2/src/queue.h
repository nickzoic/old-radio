// $Id: queue.h,v 1.2 2009-10-07 21:25:54 nick Exp $

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


queue_t *queue_init();

void queue_insert(queue_t *queue, queue_event_t event);
queue_event_t queue_pop(queue_t *queue);

void queue_destroy(queue_t *);

#endif
