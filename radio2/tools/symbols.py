#!/usr/bin/env python
# $Id: symbols.py,v 1.1 2009-10-21 22:55:28 nick Exp $
import random
import re
import sys

def bits(s):
    b = 0
    while (s):
        s &= (s - 1)
        b += 1
    return b

def findsymbols(maxones, maxzeros, minbits, maxbits, reserved):
    # Note that 0 = MARK, 1 = SPACE
    # Note that START = SPACE, END = MARK

    sre = re.compile("^(1{1,%d}0{1,%d})+$" % (maxones,maxzeros))

    symbols = []
    for symbol in range(0, 256):
        if symbol in reserved: continue
        if minbits <= bits(symbol) <= maxbits:
            x = '1'
            for bit in [1,2,4,8,16,32,64,128]:
                x += (symbol & bit) and '0' or '1'
            x += '0'
            if sre.match(x):
                symbols.append(chr(symbol))
                #sys.stderr.write("SYMBOL %02X %s %d\n" % (symbol, x, bits(symbol)));
    return symbols

def printheader(symbols, reserved):
    print "/* Autogenerated by `%s` ... do not edit */" % ' '.join(sys.argv)
    print
    print "#define SYMBOL_COUNT (%d)" % len(symbols)
    for n, r in enumerate(reserved):
        print "#define SYMBOL_RESERVED_%d (0x%02X)" % (n, r)
    print
    
    print "unsigned char symbol_encode[%d] = {" % len(symbols),
    for n, s in enumerate(symbols):
        if not n % 8: print "\n\t",
        print "0x%02X," % ord(s),
        if (n % 8 == 7): print "     /* %3d .. %3d */" % (n-7, n),
    print "\n};"
    
    decode = [ -1 ] * 256
    for n, s in enumerate(symbols):
        decode[ord(s)] = n
    for n, s in enumerate(reserved):
	decode[s] = -2 - n
    
    print
    
    print "int symbol_decode[256] = {",
    for n, d in enumerate(decode):
        if not (n % 8): print "\n\t",
        print "%3d," % d,
        if (n % 8 == 7): print "     /* %02X .. %02X */" % (n-7, n),
    print "\n};"
    

if len(sys.argv) > 1:
    maxsym = int(sys.argv[1]) if len(sys.argv) > 1 else 256
    maxones  = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    maxzeros = int(sys.argv[3]) if len(sys.argv) > 3 else maxones
    minbits = int(sys.argv[4]) if len(sys.argv) > 4 else 0
    maxbits = int(sys.argv[5]) if len(sys.argv) > 5 else 8
    reserved = [ int(x, 16) for x in sys.argv[6:] ]
   
    symbols = findsymbols(maxones, maxzeros, minbits, maxbits, reserved)

    if (8 - maxbits) < (minbits):
	symbols.reverse()
    if (maxsym < 256):
	symbols = symbols[:maxsym]

    printheader(symbols, reserved)
else:
    print "\t1x0\t2x0\t3x0\t4x0\t5x0\t6x0\t7x0\t8x0\t9x0"
    for maxones in range(1,10):
        print "%dx1:\t" % maxones,
        for maxzeros in range(1,10):
            symbols = findsymbols(maxones, maxzeros, 1, 1, []);
            print "%3d\t" % len(symbols),
        print