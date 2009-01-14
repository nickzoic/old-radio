/* $Id: sendrecv.h,v 1.2 2009-01-14 00:11:54 nick Exp $ */



int initialize_port(int fd, int baud_rate);
int send_packet(int fd, unsigned char *data, unsigned int data_length);
int recv_packet(int fd, unsigned char *data, unsigned int data_length);
unsigned char *test_ber(int fd_tx, int fd_rx, unsigned char *data, unsigned int data_length);

