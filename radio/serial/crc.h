// $Id: crc.h,v 1.1 2009-02-11 00:22:53 nick Exp $

unsigned short int crc16(unsigned char *bytes, unsigned int nbyt);
void crc16_set(unsigned char *bytes, unsigned int nbyt);
int crc16_check(unsigned char *bytes, unsigned int nbyt);
