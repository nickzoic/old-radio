// $Id: event.c,v 1.1 2009-09-24 00:21:14 nick Exp $
// Event queue: implements a simple Heap Queue for keeping events in time order

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vtime.h"
#include "loc.h"
#include "beacon.h"
#include "neigh.h"
#include "node.h"
#include "event.h"

// PRIVATE

#define EVENT_HEAP_SIZE_MIN (1024)

event_t *Event_heap = NULL;
long Event_heap_num = 0;
long Event_heap_size = 0;

#define EVENT_HEAP_PARENT(x) (((x)+1)/2-1)
#define EVENT_HEAP_LEFT(x) (((x)+1)*2-1)
#define EVENT_HEAP_RIGHT(x) (((x)+1)*2)

void event_heap_resize() {
    if (Event_heap_num == Event_heap_size) {
        Event_heap_size *= 2;
    } else if (Event_heap_num < Event_heap_size / 3 && Event_heap_size > EVENT_HEAP_SIZE_MIN) {    
        Event_heap_size /= 2;
    } else {
        return;
    }
    
    // fprintf(stderr,"event_insert: reallocating heap to %ld entries\n", Event_heap_size);
    Event_heap = (event_t *)realloc(Event_heap, Event_heap_size * sizeof(event_t));
    if (!Event_heap) {
        fprintf(stderr, "event_insert: Couldn't realloc(Event_heap, %ld * %ld)\n", Event_heap_size, sizeof(event_t));
        exit(1);
    }
}

void event_heap_heapify(int i) {
    while (1) {
        int l = EVENT_HEAP_LEFT(i);
        int r = EVENT_HEAP_RIGHT(i);
    
        int x = i;
        if (l < Event_heap_num && Event_heap[l].vtime < Event_heap[x].vtime) x = l;
        if (r < Event_heap_num && Event_heap[r].vtime < Event_heap[x].vtime) x = r;
        if (x == i) break;

        event_t temp;
        temp = Event_heap[x]; Event_heap[x] = Event_heap[i]; Event_heap[i] = temp;
        i = x;
    }
}


// PUBLIC

void event_init() {
    if (Event_heap) { free(Event_heap); }
    Event_heap_size = EVENT_HEAP_SIZE_MIN;
    Event_heap = (event_t *)malloc(Event_heap_size * sizeof(event_t));
    Event_heap_num = 0;
}

void event_insert(event_t event) {
    event_heap_resize();
    
    int i = Event_heap_num;
    Event_heap_num++;
    
    while (i && (event.vtime < Event_heap[EVENT_HEAP_PARENT(i)].vtime)) {
        Event_heap[i] = Event_heap[EVENT_HEAP_PARENT(i)];
        i = EVENT_HEAP_PARENT(i);
    }
    
    Event_heap[i] = event;
}

event_t event_pop() {
    event_t e = {VTIME_INF, NULL, NULL};
    if (!Event_heap_num) return e;
    
    e = Event_heap[0];
    Event_heap[0] = Event_heap[Event_heap_num - 1];
    Event_heap_num--;
    
    event_heap_heapify(0);
    event_heap_resize();
    
    return e;
}

void event_heap_print(long n) {
    printf("HEAP SIZE: %ld / %ld\n", Event_heap_num, Event_heap_size);
    for (long i=0; i<n && i<Event_heap_num; i++) {
        printf("%6ld: %10ld\n", i, Event_heap[i].vtime);
    }
}

void event_destroy() {
    free(Event_heap);
    Event_heap = NULL;
    Event_heap_size = Event_heap_num = 0;
}