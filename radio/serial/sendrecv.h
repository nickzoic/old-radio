/* $Id: sendrecv.h,v 1.13 2009-02-11 00:55:10 nick Exp $ */

#include <termios.h>

int initialize_port(int fd, int baud_rate);
int send_packet(int fd, unsigned char *data, unsigned int data_length);

int wait_packet(int fd, unsigned int timeout);
int recv_packet(int fd, unsigned char *data, unsigned int data_length, unsigned int timeout);

void flush_packet(int fd);

extern int baud_table[][2];

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols);
int symbols_to_bytes(unsigned char *symbols, int nsym, unsigned char *bytes);
