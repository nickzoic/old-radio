// $Id: vtime.c,v 1.3 2009-09-24 10:43:25 nick Exp $

// vtime provides a clock which will work in either the simulator (where vtime
// is used as the key of a priority queue) or in real time.

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