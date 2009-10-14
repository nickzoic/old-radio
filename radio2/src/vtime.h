// $Id: vtime.h,v 1.7 2009-10-14 05:31:18 nick Exp $

#ifndef _VTIME_H
#define _VTIME_H

#include <stdint.h>

typedef uint64_t vtime_t;

#define VTIME_ZERO ((vtime_t)0)
#define VTIME_INF ((vtime_t)-1)

#define VTIME_SECONDS (1000000L)
#define VTIME_MILLIS (1000)
#define VTIME_MICROS (1)

vtime_t vtime_zero();
vtime_t vtime_add_s(vtime_t vtime, int s);
vtime_t vtime_add_ms(vtime_t vtime, int ms);
vtime_t vtime_add_us(vtime_t vtime, int us);

#if __WORDSIZE == 64
#define VTIME_FORMAT "%012ld"
#else
#define VTIME_FORMAT "%012lld"
#endif

#endif