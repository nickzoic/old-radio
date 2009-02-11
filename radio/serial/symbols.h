/* $Id: symbols.h,v 1.8 2009-02-11 00:22:53 nick Exp $ */

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols);
int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes);

int symbol_valid(unsigned char symbol);

extern const int Symbol_Start;
extern const int Symbol_End;
extern const int Symbol_Sync;