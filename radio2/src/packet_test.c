// $Id: packet_test.c,v 1.1 2009-10-18 10:06:02 nick Exp $

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "packet.h"

#define N_CLONES (100000L)
#define N_COPIES (100000L)

int main() {
    packet_t *original = packet_new(13, "Hello, World!");
    
    
    printf("Checking Packet Cloning\n");
    
    packet_t *clones[N_CLONES];
    for (long i=0; i<N_CLONES; i++) {
        clones[i] = packet_clone(original);
        assert(clones[i]);
    }
    
    assert(original->refcount == N_CLONES + 1 );
    
    for (long i=0; i<N_CLONES; i++) {
        packet_free(clones[i]);
    }
    
    assert(original->refcount == 1);
    
    printf("Checking Packet Copying\n");
    
    packet_t *copies[N_COPIES];
    for (long i=0; i<N_COPIES; i++) {
        copies[i] = packet_copy(original);
    }
    
    assert(original->refcount == 1);
    
    for (long i=0; i<N_COPIES; i++) {
        packet_free(copies[i]);
    }
    
    packet_free(original);
    
    printf("Success!\n\n");
    return 0;
}