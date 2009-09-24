#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vtime.h"
#include "loc.h"
#include "beacon.h"
#include "neigh.h"
#include "node.h"
#include "event.h"

#define N_TEST (1000000L)

int main() {
    
    event_init();
    
    for (int i=0; i<N_TEST; i++) {
        event_t ee = { 0 };
        ee.vtime = rand();
        event_insert(ee);
    }
    
    event_heap_print(5);
    
    vtime_t vtime = 0;
    for (int i=0; i<N_TEST; i++) {
        event_t ee = event_pop();
        if (ee.vtime == VTIME_INF) {
            fputs("STACK UNDERFLOW!\n", stderr);
            return 1;
        }
        if (ee.vtime < vtime) {
            fputs("TIME OUT OF JOINT!\n", stderr);
            return 2;
        }
        vtime = ee.vtime;
    }
    
    event_t ee = event_pop();
    if (ee.vtime != VTIME_INF) {
        fputs("STACK OVERFLOW!\n", stderr);
        return 3;
    }
    
    event_destroy();
    
    return 0;
}