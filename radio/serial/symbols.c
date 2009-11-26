/* $Id: symbols.c,v 1.11 2009-11-26 07:52:32 nick Exp $ */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "symbols.h"

#include "symbols.i"

#if SYMBOL_COUNT < 64
#error symbols.c needs >= 64 symbols!
#endif

#if SYMBOL_COUNT < 128
#define SYMBOL_BITS (6)
#define SYMBOL_MASK (0x3F)
#define SYMBOL_CHUNK_SYMS (4)
#define SYMBOL_CHUNK_BYTES (3)
#else
#define SYMBOL_BITS (7)
#define SYMBOL_MASK (0x7F)
#define SYMBOL_CHUNK_SYMS (8)
#define SYMBOL_CHUNK_BYTES (7)
#endif

#ifndef SYMBOL_RESERVED_1
#error symbols.c needs 2 reserved symbols!
#endif

#define MIN(x,y) ((x) < (y))?(x):(y)

const int Symbol_Start = SYMBOL_RESERVED_0;
const int Symbol_End = SYMBOL_RESERVED_1;
const int Symbol_Sync = SYMBOL_RESERVED_2;

// pack N bytes into N+1 symbols

int _b2s(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    if (nbytes == 0) return 0;
    assert(nbytes <= 8);
    uint64_t temp = 0;
    for (int i = 0; i < nbytes; i++) {
        temp += (uint64_t)bytes[i] << (8 * i);
    }
    for (int i = 0; i < nbytes + 1; i++) {
        symbols[i] = symbol_encode[ (temp >> (SYMBOL_BITS * i)) & SYMBOL_MASK ];
    }
    return nbytes + 1;
}

// pack a whole string of bytes into symbols.
// XXX TOFIX: May buffer overrrun if symbols[] isn't big enough

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    int nsym = 0;
    for (int i = 0; i < nbytes; i += SYMBOL_CHUNK_BYTES)
        nsym += _b2s(bytes+i, MIN(nbytes-i, SYMBOL_CHUNK_BYTES), symbols+nsym);
    return nsym;
}

// unpack N symbols into N-1 bytes

int _s2b(unsigned char *symbols, int nsym, unsigned char *bytes) {
    assert(nsym * SYMBOL_BITS < 64);
    
    if (nsym < 2) return 0;
    uint64_t temp = 0;
    
    for (int i=0; i < nsym; i++) {
        temp += (uint64_t)symbol_decode[symbols[i]] << (SYMBOL_BITS * i);
    }
    for (int i=0; i < nsym - 1; i++) {
        bytes[i] = (temp >> (8 * i)) & 0xFF;
    }
    return nsym - 1;
}

// unpack a whole string of symbols into bytes.
// XXX TOFIX: May buffer overrrun if bytes[] isn't big enough

int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes) {
    int nbyt = 0;
    for (int i = 0; i < nsym; i += SYMBOL_CHUNK_SYMS) {
        int x = _s2b(symbols+i, MIN(nsym-i, SYMBOL_CHUNK_SYMS), bytes+nbyt);
        nbyt += x;
        if (x < SYMBOL_CHUNK_BYTES) return nbyt;
    }
    return nbyt;
}

// check a single symbol for validity.

int symbol_valid(unsigned char symbol) {
    return (symbol_decode[symbol]>=0);
}

////////////////////////////////////////////////////////////////////////////////

