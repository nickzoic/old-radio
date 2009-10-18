// $Id: crc_test.c,v 1.1 2009-10-18 05:54:44 nick Exp $

#include <stdio.h>
#include <string.h>

#include "crc.h"

int main() {
    printf("Checking CRC16(\"123456789\") == 0x29B1\n");
    
    unsigned char teststring[] = "123456789__";
    
    unsigned int crc = crc16(teststring, 9);
    if (crc != 0x29B1) {
        printf("crc16 FAIL %04X\n", crc);
        return 1;
    }
    crc16_set(teststring, 11);
    
    if (memcmp(teststring, "123456789\xB1\x29", 11)) {
        printf("crc16_set FAIL %02X %02X.\n", teststring[9], teststring[10]);
        return 2;
    }
    
    if (!crc16_check(teststring, 11)) {
        printf("crc16_check FAIL.\n");
        return 3;
    }
    
    printf("Success!\n\n");
    return 0;
}