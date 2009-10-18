// $Id: radio.h,v 1.1 2009-10-18 04:34:04 nick Exp $

#ifndef _RADIO_H
#define _RADIO_H

#include <termios.h>

#include "packet.h"
#include "vtime.h"

#define RADIO_MTU (1600)

typedef struct radio_s {
    int fd;
} radio_t;

radio_t *radio_new(char *devname, int baud_rate);

void radio_flush(radio_t *radio);

void radio_send(radio_t *radio, packet_t *packet);

int radio_wait(radio_t *radio, vtime_t timeout);

packet_t *radio_recv(radio_t *radio, vtime_t timeout);

void radio_free(radio_t *radio);

#endif