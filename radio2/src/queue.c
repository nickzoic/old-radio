// $Id: queue.c,v 1.5 2009-10-21 12:24:17 nick Exp $
// queue: implements a simple Heap Queue for keeping queue_events in time order
// It is implemented as an automatically resizing heap queue.

#include <stdlib.h>
#include <assert.h>

#include "queue.h"

////////////////////////////////////////////////////////////////////////////////
// PRIVATE

#define QUEUE_HEAP_SIZE_MIN (1024)

#define QUEUE_HEAP_PARENT(x) (((x)+1)/2-1)
#define QUEUE_HEAP_LEFT(x) (((x)+1)*2-1)
#define QUEUE_HEAP_RIGHT(x) (((x)+1)*2)

////////////////////////////////////////////////////////////////////////////////

void _queue_resize(queue_t *queue) {
    if (queue->nevents == queue->size) {
        queue->size *= 2;
    } else if (queue->nevents < queue->size / 3 && queue->size > QUEUE_HEAP_SIZE_MIN) {    
        queue->size /= 2;
    } else {
        return;
    }
    
    queue->events = (queue_event_t *)realloc(queue->events, queue->size * sizeof(queue_event_t));
    assert(queue->events);
}

////////////////////////////////////////////////////////////////////////////////

void _queue_heapify(queue_t *queue) {
    int i = 0;
    while (1) {
        int l = QUEUE_HEAP_LEFT(i);
        int r = QUEUE_HEAP_RIGHT(i);
        
        int x = i;
        if (l < queue->nevents && queue->events[l].vtime < queue->events[x].vtime) x = l;
        if (r < queue->nevents && queue->events[r].vtime < queue->events[x].vtime) x = r;
        if (x == i) break;
        assert (x < queue->nevents);
        
        queue_event_t temp = queue->events[x];
        queue->events[x] = queue->events[i];
        queue->events[i] = temp;
        
        i = x;
    }
}

// PUBLIC
////////////////////////////////////////////////////////////////////////////////

queue_t *queue_new() {
    
    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
    queue->size = QUEUE_HEAP_SIZE_MIN;
    queue->nevents = 0;
    queue->events = (queue_event_t *)malloc(queue->size * sizeof(queue_event_t));
    assert(queue->events);
    
    return queue;
}

////////////////////////////////////////////////////////////////////////////////

void queue_insert(queue_t *queue, queue_event_t event) {
    _queue_resize(queue);
    
    int i = queue->nevents;
    queue->nevents++;
    
    while (i && (event.vtime < queue->events[QUEUE_HEAP_PARENT(i)].vtime)) {
        queue->events[i] = queue->events[QUEUE_HEAP_PARENT(i)];
        i = QUEUE_HEAP_PARENT(i);
        assert(i >= 0);
    }
    
    queue->events[i] = event;
}

////////////////////////////////////////////////////////////////////////////////

queue_event_t queue_pop(queue_t *queue) {
    queue_event_t e = QUEUE_EVENT_NULL;
    if (!queue->nevents) return e;
    
    e = queue->events[0];
    queue->events[0] = queue->events[queue->nevents - 1];
    queue->nevents--;
    
    _queue_heapify(queue);
    _queue_resize(queue);
    
    return e;
}

////////////////////////////////////////////////////////////////////////////////

void queue_free(queue_t *queue) {
    free(queue->events);
    free(queue);
}
