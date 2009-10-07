// $Id: packet.c,v 1.1 2009-10-07 21:18:05 nick Exp $

#include <assert.h>
#include <string.h>

#include "packet.h"

packet_t *packet_new(size_t length, void *data) {
    assert(length);
    assert(data);
    
    packet_t *p = (packet_t *)malloc(sizeof(packet_t));
    assert(p);
    
    p->length = length;
    p->data = (unsigned char *)malloc(length);
    assert(p->data);
    
    memcpy(p->data, data, length);
    return p;
}

packet_t *packet_copy(packet_t *p) {
    return packet_new(p->length, p->data);
}

packet_t *packet_clone(packet_t *p) {
    // this should do a refcount instead
    return packet_copy(p);
}

void packet_free(packet_t *p) {
    assert(p->data);
    free(p->data);
    free(p);
}