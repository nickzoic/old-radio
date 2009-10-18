// $Id: queue_test.c,v 1.2 2009-10-18 05:54:44 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <sys/time.h>

#include "queue.h"

#define N_TEST (1000000L)

int main() {
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec * getpid());

    printf("Testing queue ...\n");
    
    queue_t *queue = queue_new();

    printf("Inserting %ld entries ", N_TEST);
    
    for (int i=0; i<N_TEST; i++) {
        queue_event_t e = { 0 };
        e.vtime = rand();
        queue_insert(queue, e);
        if (i % 100000 == 0) {
            putchar('.');  
	    fflush(stdout);
        }
    }
    
    printf("\nPopping %ld entries ", N_TEST);
    
    vtime_t vtime = 0;
    for (int i=0; i<N_TEST; i++) {
        queue_event_t e = queue_pop(queue);
        
        assert(e.vtime != VTIME_INF);
        assert(e.vtime >= vtime);
        
        vtime = e.vtime;
        
        if (i % 100000 == 0) {
            putchar('.');  
	    fflush(stdout);
        }
    }
    
    printf("\nTesting underflow\n");
    
    queue_event_t e = queue_pop(queue);
    
    assert(e.vtime == VTIME_INF);
    
    queue_free(queue);
    
    printf ("Success!\n\n");
    return 0;
}