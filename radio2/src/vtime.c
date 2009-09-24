// $Id: vtime.c,v 1.2 2009-09-24 00:26:43 nick Exp $

#include <stdint.h>
#include "vtime.h"

#include <time.h>
#include <sys/time.h>

// Make vtimes slightly easier to look at by setting a more recent epoch.
#define VTIME_EPOCH (0x4ACE6305)  // 2009-09-09 09:09:09

vtime_t vtime_from_wall() {
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    return (tv.tv_sec - VTIME_EPOCH) * 1000000LL + tv.tv_usec;
}

vtime_t vtime_zero() {
    return 0;
}

vtime_t vtime_add_s(vtime_t vtime, int s) {
    return vtime + s * 1000000L;
}

vtime_t vtime_add_ms(vtime_t vtime, int ms) {
    return vtime + ms * 1000;
}