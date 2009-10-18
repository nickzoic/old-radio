/* $Id: symbols.c,v 1.1 2009-10-18 05:54:45 nick Exp $ */

#include <stdio.h>
#include <stdint.h>

#include "symbols.h"
#include "symbols.i"

#if SYMBOL_COUNT < 128
#error symbols.c needs >= 128 symbols!
#endif

#ifndef SYMBOL_RESERVED_2
#error symbols.c needs 3 reserved symbols!
#endif

const int Symbol_Start = SYMBOL_RESERVED_0;
const int Symbol_End = SYMBOL_RESERVED_1;
const int Symbol_Sync = SYMBOL_RESERVED_2;

// pack up to 7 bytes into up to 8 symbols.

int _b2s(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    if (nbytes == 0) return 0;
    uint64_t temp = 0;
    for (int i = 0; i < nbytes; i++) {
        temp += (uint64_t)bytes[i] << (8 * i);
    }
    for (int i = 0; i < nbytes + 1; i++) {
        symbols[i] = symbol_encode[ (temp >> (7 * i)) & 0x7F ];
    }
    return nbytes + 1;
}

// pack a whole string of bytes into symbols.
// XXX TOFIX: May buffer overrrun if symbols[] isn't big enough

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    int nsym = 0;
    for (int i = 0; i < nbytes; i += 7)
        nsym += _b2s(bytes+i, (nbytes-i<7)?(nbytes-i):7, symbols+nsym);
    return nsym;
}

// unpack up to 7 bytes from up to 8 symbols

int _s2b(unsigned char *symbols, int nsym, unsigned char *bytes) {
    if (nsym < 2) return 0;
    uint64_t temp = 0;
    
    for (int i=0; i < nsym; i++) {
        temp += (uint64_t)symbol_decode[symbols[i]] << (7 * i);
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
    for (int i = 0; i < nsym; i += 8) {
        int x = _s2b(symbols+i, (nsym-i<8)?(nsym-i):8, bytes+nbyt);
        nbyt += x;
        if (x < 7) return nbyt;
    }
    return nbyt;
}

// check a single symbol for validity.

int symbol_valid(unsigned char symbol) {
    return (symbol_decode[symbol]>=0);
}

////////////////////////////////////////////////////////////////////////////////

