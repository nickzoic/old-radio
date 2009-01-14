/* $Id: sendrecv.h,v 1.3 2009-01-14 03:02:22 nick Exp $ */

#include <termios.h>

int initialize_port(int fd, int baud_rate);
int send_packet(int fd, unsigned char *data, unsigned int data_length);
int recv_packet(int fd, unsigned char *data, unsigned int data_length);
unsigned char *test_ber(int fd_tx, int fd_rx, unsigned char *data, unsigned char *recv, unsigned int data_length);

extern int baud_table[][2];
