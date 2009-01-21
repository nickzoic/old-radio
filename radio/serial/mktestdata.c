/* $Id: mktestdata.c,v 1.2 2009-01-21 10:38:00 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include "symbols.h"

int main() {
    for (int i=0; i<10240; i++) {
        int n = rand() % SYMBOL_COUNT;
	putchar(symbol_encode[n]);
    }
    
    return 0;
}
