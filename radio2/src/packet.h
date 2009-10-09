// $Id: packet.h,v 1.6 2009-10-09 10:00:13 nick Exp $

#ifndef _PACKET_H
#define _PACKET_H

#include <stdlib.h>
#include <stdint.h>

#include "loc.h"

typedef struct packet_s {
    size_t length;
    size_t refcount;
    unsigned char *data;
} packet_t;

packet_t *packet_new(size_t length, void *data);
packet_t *packet_copy(packet_t *p);
packet_t *packet_clone(packet_t *p);
void packet_free(packet_t *p);

#define PACKET_TYPE_BEACON (0xBE)
#define PACKET_TYPE_FLOOD (0xF0)
#define PACKET_TYPE_DATA (0xDA)

#endif