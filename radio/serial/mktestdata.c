/* $Id: mktestdata.c,v 1.1 2009-01-14 07:55:08 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>

int main() {
    int i;
    
    for (i=0; i<10240; i++) {
        int n = random() & 0x55;
        n *= 3;
        n ^= 0xAA;
        
        /* int n = (random() % 254) + 1; */
        
        putchar(n);        
    }
    
    return 0;
}