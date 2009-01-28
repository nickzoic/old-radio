/* $Id: symbols.h,v 1.3 2009-01-28 04:56:53 nick Exp $ */

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols);
int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes);

int symbol_valid(unsigned char symbol);

extern const int Symbol_Start;
extern const int Symbol_End;
extern const int Symbol_Sync;