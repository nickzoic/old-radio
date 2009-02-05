/* $Id: test_symbols.c,v 1.6 2009-02-05 01:49:11 nick Exp $ */
/* Test a ridiculously large number of byte strings of varying length
 * to make sure they round-trip properly through bytes_to_symbols and
 * symbols_to_bytes.  
 */

#include <stdio.h>
#include <stdlib.h>

#include "sendrecv.h"

extern int symbol_decode[256];

void print_symbol(int sym) {
    int x = symbol_decode[sym];
    char x0 = (x & 0x01)?'1':'0';	
    char x1 = (x & 0x02)?'1':'0';	
    char x2 = (x & 0x04)?'1':'0';	
    char x3 = (x & 0x08)?'1':'0';	
    char x4 = (x & 0x10)?'1':'0';	
    char x5 = (x & 0x20)?'1':'0';	
    char x6 = (x & 0x40)?'1':'0';	
    printf("S %02X %3d %c%c%c%c%c%c%c\n", sym, x, x0, x1, x2, x3, x4, x5, x6);
}

void print_byte(unsigned char byt) {
    char b0 = (byt & 0x01)?'1':'0';	
    char b1 = (byt & 0x02)?'1':'0';	
    char b2 = (byt & 0x04)?'1':'0';	
    char b3 = (byt & 0x08)?'1':'0';	
    char b4 = (byt & 0x10)?'1':'0';	
    char b5 = (byt & 0x20)?'1':'0';	
    char b6 = (byt & 0x40)?'1':'0';	
    char b7 = (byt & 0x80)?'1':'0';	
    printf("B %02X %c%c%c%c%c%c%c%c\n", byt, b0, b1, b2, b3, b4, b5, b6, b7);
}

int main(int argc, char **argv) {

    unsigned char sym[800];
    unsigned char byte1[701];
    unsigned char byte2[701];

    printf("Testing Symbols");
    for (long int c=0; c<100000; c++) {
	if (c % 2000 == 0) {
		putchar('.');  
		fflush(stdout);
	}
	int len = rand() % 700;
	
	for (int i=0; i<len; i++) {
	    byte1[i] = rand() & 0xFF;
	}
	
	int nsym = bytes_to_symbols(byte1, len, sym);
	int nbyt = symbols_to_bytes(sym, nsym, byte2);

	int pass = 0;
	if (nbyt == len) {
	    pass = 1;
	    for (int i=0; i<len; i++) {
	        if (byte1[i] != byte2[i]) {
		    pass = 0;
	        }
	    }
	}
	if (!pass) {
	    printf ("\n\nFAILURE!\n");
	    printf ("BYTE1 %d\n", len);
	    for (int i=0; i<len; i++) {
		print_byte(byte1[i]);
	    }
	    printf ("SYM %d\n", nsym);
	    for (int i=0; i<nsym; i++) {
		print_symbol(sym[i]);
	    }
	    printf ("BYTE2 %d\n", nbyt);
	    for (int i=0; i<nbyt; i++) {
		print_byte(byte2[i]);
	    }
	    exit(1);
	}
    }
    printf("\nSuccess\n");
    
    printf("Checking CRC16(\"123456789\") == 0x29B1\n");
    unsigned short int crc = crc16("123456789", 9);
    printf("=> %04X\n%s\n", crc, (crc == 0x29B1)?"Success":"Failure");
    
    return 0;
}
