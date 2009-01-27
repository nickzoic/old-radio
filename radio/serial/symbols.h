/* $Id: symbols.h,v 1.2 2009-01-27 23:59:18 nick Exp $ */

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols);
int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes);

extern const int Symbol_Start;
extern const int Symbol_End;
extern const int Symbol_Sync;