// $Id: packet.h,v 1.1 2009-10-07 21:18:05 nick Exp $

#ifndef _PACKET_H
#define _PACKET_H

#include <stdlib.h>

typedef struct packet_s {
    size_t length;
    unsigned char *data;
} packet_t;

packet_t *packet_new(size_t length, void *data);
packet_t *packet_copy(packet_t *p);
packet_t *packet_clone(packet_t *p);
void packet_free(packet_t *p);

#endif