// $Id: vtime.h,v 1.10 2009-11-17 03:51:43 nick Exp $

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
#define VTIME_FORMAT "%012lu"
#else
#define VTIME_FORMAT "%012llu"
#endif

#define VTIME_FORMAT_STR "%06lu.%06lu"
#define VTIME_FORMAT_ARG(x) ((unsigned long)(x)/VTIME_SECONDS),((unsigned long)(x)%VTIME_SECONDS)

#endif