// $Id: vtime.h,v 1.8 2009-10-18 05:28:07 nick Exp $

#ifndef _VTIME_H
#define _VTIME_H

#include <stdint.h>

typedef uint64_t vtime_t;

#define VTIME_ZERO ((vtime_t)0)
#define VTIME_INF ((vtime_t)-1)

#define VTIME_SECONDS (1000000L)
#define VTIME_MILLIS (1000)
#define VTIME_MICROS (1)

vtime_t vtime_from_wall();

#if __WORDSIZE == 64
#define VTIME_FORMAT "%012ld"
#else
#define VTIME_FORMAT "%012lld"
#endif

#endif