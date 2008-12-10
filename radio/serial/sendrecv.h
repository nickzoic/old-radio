/* $Id: sendrecv.h,v 1.1 2008-12-10 05:43:58 nick Exp $ */



int initialize_port(int fd, int baud_rate);
int send_packet(int fd, unsigned char *data, unsigned int data_length);
int recv_packet(int fd, unsigned char *data, unsigned int data_length);
unsigned char *test_ber(int fd_tx, int fd_rx, unsigned char *data, unsigned int data_length);

#define NUM_BAUD_RATES (4)
static int baud_rates[NUM_BAUD_RATES] = { 1200, 1800, 2400, 4800 };

