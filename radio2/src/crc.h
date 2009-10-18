// $Id: crc.h,v 1.1 2009-10-18 05:54:44 nick Exp $

#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>
#include <stdlib.h>

#define CRC16_LEN (2)

uint16_t crc16(unsigned char *bytes, size_t nbyt);
void crc16_set(unsigned char *bytes, size_t nbyt);
int crc16_check(unsigned char *bytes, size_t nbyt);

#endif