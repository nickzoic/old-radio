/* $Id: symbols.c,v 1.4 2009-02-05 01:48:07 nick Exp $ */

#include <stdio.h>

#include "symbols.h"

#include "symbols.i"

#if SYMBOL_COUNT < 128
#error symbols.c needs >= 128 symbols!
#endif

#ifndef SYMBOL_RESERVED_1
#error symbols.c needs 2 reserved symbols!
#endif

const int Symbol_Start = SYMBOL_RESERVED_0;
const int Symbol_End = SYMBOL_RESERVED_1;
const int Symbol_Sync = SYMBOL_RESERVED_2;

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
    int nsym = 0;
    for (int i = 0; i < nbytes; i += 7)
        nsym += _b2s(bytes+i, (nbytes-i<7)?(nbytes-i):7, symbols+nsym);
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

int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes) {
    int nbyt = 0;
    for (int i = 0; i < nsym; i += 8) {
        int x = _s2b(symbols+i, (nsym-i<8)?(nsym-i):8, bytes+nbyt);
        nbyt += x;
        if (x < 7) return nbyt;
    }
    return nbyt;
}

int symbol_valid(unsigned char symbol) {
    return (symbol_decode[symbol]>=0);
}

////////////////////////////////////////////////////////////////////////////////

// CRC algorithm as cargo-culted from pycrc.py
// At least approximately a 16-bit wide CCITT CRC.

static const unsigned short int crc_table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

unsigned short int crc16(unsigned char *data, unsigned int nbyt) {
    unsigned short int crc = 0xFFFF;
    while (nbyt--) {
        int tbl_idx = ((crc >> 8) ^ *data) & 0xff;
        crc = (crc_table[tbl_idx] ^ (crc << 8)) & 0xffff;
        data++;
    }
    return crc;
}