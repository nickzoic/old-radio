// $Id: packet.c,v 1.2 2009-10-07 21:21:17 nick Exp $

#include <assert.h>
#include <string.h>

#include "packet.h"

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

packet_t *packet_copy(packet_t *p) {
    assert(p);
    return packet_new(p->length, p->data);
}

packet_t *packet_clone(packet_t *p) {
    assert(p && p->refcount);
    p->refcount++;
    return p;
}

void packet_free(packet_t *p) {
    assert(p && p->refcount);
    p->refcount--;
    if (p->refcount == 0) {
        free(p->data);
        free(p);
    }
}