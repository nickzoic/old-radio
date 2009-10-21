// $Id: packet.c,v 1.3 2009-10-21 12:24:16 nick Exp $

#include <assert.h>
#include <string.h>

#include "packet.h"

////////////////////////////////////////////////////////////////////  packet_new

packet_t *packet_new(size_t length, void *data) {
    assert(length);
    assert(data);
    
    packet_t *p = (packet_t *)malloc(sizeof(packet_t));
    assert(p);
    
    p->length = length;
    p->refcount = 1;
    
    p->data = (unsigned char *)malloc(length);
    assert(p->data);
    
    memcpy(p->data, data, length);
    return p;
}

///////////////////////////////////////////////////////////////////  packet_copy

packet_t *packet_copy(packet_t *p) {
    assert(p);
    return packet_new(p->length, p->data);
}

//////////////////////////////////////////////////////////////////  packet_clone

packet_t *packet_clone(packet_t *p) {
    assert(p && p->refcount);
    p->refcount++;
    return p;
}

///////////////////////////////////////////////////////////////////  packet_free

void packet_free(packet_t *p) {
    assert(p && p->refcount);
    p->refcount--;
    if (p->refcount == 0) {
        free(p->data);
        free(p);
    }
}