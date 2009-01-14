/* $Id: test_ber.c,v 1.3 2009-01-14 03:02:22 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "sendrecv.h"

unsigned char testdata[10240];
unsigned char recvdata[10240];

int bit_compare(unsigned char *a, unsigned char *b, unsigned int len) {
    int i, r=0;
    for (i=0; i<len; i++) {
        unsigned char d = a[i] ^ b[i];
        while (d) {
            d &= d - 1;
            r++;
        }
    }
    return r;
}

int die(char *s) {
    fprintf(stderr, "DIE %s\n", s);
    exit(1);
}

unsigned char *test_ber(int fd_tx, int fd_rx, unsigned char *data, unsigned char *recv, unsigned int data_length)
{

    struct pollfd pollfds[2] = {
        { fd_rx, POLLIN, 0 } ,
	{ fd_tx, POLLOUT, 0 } ,
    };

    int n_sent = 0;
    int n_recv = 0;

    int state = 0;
    
    if (!recv) recv = (unsigned char *)calloc(data_length,1);
    
    while ((n_sent < data_length) || (n_recv < data_length)) {
	pollfds[0].events = (n_recv < data_length)?POLLIN:0;
	pollfds[1].events = (n_sent < data_length)?POLLOUT:0;

	int e = poll(pollfds, 2, 1000);
	if (e == 0) break;

	if (pollfds[0].revents & POLLIN) {
            switch (state) {
                case 0:
                    e = read(fd_rx, recv, 1);
                    if (e == 1 && recv[0] == 0x55) state = 1;
                    break;
                case 1:
                case 2:
                    e = read(fd_rx, recv, 1);
                    if (e == 1 && recv[0] == 0x00) state = 3;
                    break;
                case 3:
                    e = read(fd_rx, recv + n_recv, data_length - n_recv);
                    if (e>0) n_recv += e;
            }
            fprintf(stderr, "STATE %d GOT %02X\n", state, recv[0]);
                    
	}

	if (pollfds[1].revents & POLLOUT) {
            switch (state) {
                case 0:
                    e = write(fd_tx, "\x55", 1);
                    break;
                case 1:
                    e = write(fd_tx, "\xFF\xFF\x00", 3);
                    state = 2;
                    break;
                case 2:
                case 3:
                    e = write(fd_tx, data + n_sent, data_length - n_sent);
                    if (e>0) n_sent += e;
            }
	}
                
    }
    
    return recv;
}

int main(int argc, char **argv) {

    if (argc<3) {
        fprintf(stderr, "usage: %s <txdevname> <rxdevname>\n", argv[0]);
        exit(1);
    }

    int fd_tx = open(argv[1], O_WRONLY | O_NONBLOCK);
    int fd_rx = open(argv[2], O_RDONLY | O_NONBLOCK);
    
    if (fd_tx < 0 || fd_rx < 0) {
        fprintf(stderr, "couldn't open device: %s", strerror(errno));
        exit(1);
    }
    
    FILE *fp = fopen("test_ber.data", "rb");
    if (fread(testdata, 1, sizeof(testdata), fp) != sizeof(testdata)) {
        fprintf(stderr, "couldn't read test_ber.data: %s", strerror(errno));
        exit(2);
    }
    fclose(fp);
 
    int i;
    for (i=0; baud_table[i][1]; i++) {
        int baud_num = baud_table[i][0];
        int baud_rate = baud_table[i][1];
        
        initialize_port(fd_rx, baud_num);
        initialize_port(fd_tx, baud_num);
    
        test_ber(fd_tx, fd_rx, testdata, recvdata, sizeof(testdata));
        int errors = bit_compare(testdata, recvdata, sizeof(testdata));
    
        printf("BAUD %d ERRORS %d BER %f\n", baud_rate, errors, (double)errors/(sizeof(testdata)*8));
    }
        
    return 0;
}
