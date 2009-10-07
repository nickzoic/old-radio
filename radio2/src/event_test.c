#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "event.h"

#define N_TEST (10000000L)

int main() {
    
    event_init();
    
    for (int i=0; i<N_TEST; i++) {
        event_t ee = { 0 };
        ee.vtime = rand();
        event_insert(ee);
    }
    
    vtime_t vtime = 0;
    for (int i=0; i<N_TEST; i++) {
        event_t ee = event_pop();
        
        assert(ee.vtime != VTIME_INF);
        assert(ee.vtime >= vtime);
        
        vtime = ee.vtime;
    }
    
    event_t ee = event_pop();
    
    assert(ee.vtime == VTIME_INF);
    
    event_destroy();
    
    return 0;
}