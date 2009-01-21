/* $Id: sendrecv.h,v 1.5 2009-01-21 10:38:00 nick Exp $ */

#include <termios.h>

int initialize_port(int fd, int baud_rate);
int send_packet(int fd, unsigned char *data, unsigned int data_length);
int recv_packet(int fd, unsigned char *data, unsigned int data_length);

void flush_packet(int fd);

extern int baud_table[][2];
