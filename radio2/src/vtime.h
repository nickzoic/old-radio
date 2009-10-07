// $Id: vtime.h,v 1.5 2009-10-07 22:06:15 nick Exp $

#ifndef _VTIME_H
#define _VTIME_H

#include <stdint.h>

typedef uint64_t vtime_t;

#define VTIME_INF ((vtime_t)-1)

#define VTIME_SECONDS (1000000L)
#define VTIME_MILLIS (1000)
#define VTIME_MICROS (1)

#if __WORDSIZE == 64
#define VTIME_FORMAT "%09ld"
#else
#define VTIME_FORMAT "%09lld"
#endif

#endif