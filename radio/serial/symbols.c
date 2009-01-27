/* $Id: symbols.c,v 1.1 2009-01-27 21:55:52 nick Exp $ */

#include "symbols.h"

#include "symbols.i"

#if SYMBOL_COUNT < 128
#error Need 128 Symbols
#endif

#define BITMASK(b) ( (1 << (b)) - 1 )
#define GETBITS(x,a,b) ( ((x) >> (a)) & BITMASK(b) )

// pack up to 7 bytes into up to 8 symbols.
// XXX TOFIX: May actually look one byte past the nbytes specified

int _b2s(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    if (nbytes < 1) return 0;
    symbols[0] = symbol_encode[                             GETBITS(bytes[0], 0, 7)       ];
    symbols[1] = symbol_encode[ GETBITS(bytes[0], 7, 1) + ( GETBITS(bytes[1], 0, 6) << 1 )];
    if (nbytes < 2) return 2;
    symbols[2] = symbol_encode[ GETBITS(bytes[1], 6, 2) + ( GETBITS(bytes[2], 0, 5) << 2 )];
    if (nbytes < 3) return 3;
    symbols[3] = symbol_encode[ GETBITS(bytes[2], 5, 3) + ( GETBITS(bytes[3], 0, 4) << 3 )];
    if (nbytes < 4) return 4;
    symbols[4] = symbol_encode[ GETBITS(bytes[3], 4, 4) + ( GETBITS(bytes[4], 0, 3) << 4 )];
    if (nbytes < 5) return 5;
    symbols[5] = symbol_encode[ GETBITS(bytes[4], 3, 5) + ( GETBITS(bytes[5], 0, 2) << 5 )];
    if (nbytes < 6) return 6;
    symbols[6] = symbol_encode[ GETBITS(bytes[5], 2, 6) + ( GETBITS(bytes[6], 0, 1) << 6 )];
    if (nbytes < 7) return 7;
    symbols[7] = symbol_encode[ GETBITS(bytes[6], 1, 7)                                   ];
    return 8;
}

// pack a whole string of bytes into symbols.
// XXX TOFIX: May buffer overrrun if symbols[] isn't big enough

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    nsym = 0;
    for (int i = 0; i < nbytes; i += 8)
        nsym += _b2s(bytes+i, (nbytes-i<8)?(nbytes-i):8, symbols+nsym)
    return nsym;
}

// unpack up to 7 bytes from up to 8 symbols

int _s2b(unsigned char *symbols, int nsym, unsigned char *bytes) {
   
    if (nsym < 2) return 0; 
    int s0 = symbol_decode[symbols[0]];
    int s1 = symbol_decode[symbols[1]];
    if (s0 == -1 || s1 == -1) return 0;
    bytes[0] = GETBITS(s0, 0, 7) + ( GETBITS(s1, 0, 1) << 7);
   
    if (nsym < 3) return 1; 
    int s2 = symbol_decode[symbols[2]];
    if (s2 == -1) return 1;
    bytes[1] = GETBITS(s1, 1, 6) + ( GETBITS(s2, 0, 2) << 6);
    
    if (nsym < 4) return 2; 
    int s3 = symbol_decode[symbols[3]];
    if (s3 == -1) return 2;
    bytes[2] = GETBITS(s2, 2, 5) + ( GETBITS(s3, 0, 3) << 5);
    
    if (nsym < 5) return 3; 
    int s4 = symbol_decode[symbols[4]];
    if (s4 == -1) return 3;
    bytes[3] = GETBITS(s3, 3, 4) + ( GETBITS(s4, 0, 4) << 4);
    
    if (nsym < 6) return 4; 
    int s5 = symbol_decode[symbols[5]];
    if (s5 == -1) return 4;
    bytes[4] = GETBITS(s4, 4, 3) + ( GETBITS(s5, 0, 5) << 3);
    
    if (nsym < 7) return 5; 
    int s6 = symbol_decode[symbols[6]];
    if (s6 == -1) return 5;
    bytes[5] = GETBITS(s5, 5, 2) + ( GETBITS(s6, 0, 6) << 2);
    
    if (nsym < 8) return 6; 
    int s7 = symbol_decode[symbols[7]];
    if (s7 == -1) return 6;
    bytes[6] = GETBITS(s6, 6, 1) + ( GETBITS(s7, 0, 7) << 1);

    return 7;
}

// unpack a whole string of symbols into bytes.
// XXX TOFIX: May buffer overrrun if bytes[] isn't big enough

int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes);
    nbyt = 0;
    for (int i = 0; i < nbytes; i += 7)
        nsym += _b2s(symbols+i, (nbyt-i<7)?(nbyt-i):7, bytes+nbyt)
    return nbyt;
}
