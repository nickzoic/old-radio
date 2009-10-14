#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "queue.h"

#define N_TEST (1000000L)

int main() {
    
    queue_t *queue = queue_new();
    
    for (int i=0; i<N_TEST; i++) {
        queue_event_t e = { 0 };
        e.vtime = rand();
        queue_insert(queue, e);
    }
    
    vtime_t vtime = 0;
    for (int i=0; i<N_TEST; i++) {
        queue_event_t e = queue_pop(queue);
        
        assert(e.vtime != VTIME_INF);
        assert(e.vtime >= vtime);
        
        vtime = e.vtime;
    }
    
    queue_event_t e = queue_pop(queue);
    
    assert(e.vtime == VTIME_INF);
    
    queue_free(queue);
    
    return 0;
}