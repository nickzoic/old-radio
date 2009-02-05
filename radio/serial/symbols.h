/* $Id: symbols.h,v 1.7 2009-02-05 01:49:11 nick Exp $ */

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols);
int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes);

int symbol_valid(unsigned char symbol);

unsigned short int crc16(unsigned char *data, unsigned int nbyt);

extern const int Symbol_Start;
extern const int Symbol_End;
extern const int Symbol_Sync;