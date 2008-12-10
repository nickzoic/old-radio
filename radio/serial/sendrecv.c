/* $Id: sendrecv.c,v 1.1 2008-12-10 05:43:58 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <poll.h>

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "sendrecv.h"

#define PREAMBLE_LEN (12)
#define PREAMBLE_BYTE (0x55)
#define UARTSYNC_LEN (2)
#define UARTSYNC_BYTE (0xFF)

int baud_numbers[NUM_BAUD_RATES] = { B1200, B1800, B2400 };

int initialize_port(int fd, int baud_rate) {
    int i;
    int baud_number = B2400;
    /*for (i=0; i<NUM_BAUD_RATES; i++) {
        if (baud_rates[i] == baud_rate) break;
    }
    if (i>=NUM_BAUD_RATES) {
        fprintf(stderr, "WARNING: Invalid baud rate: %d\n", baud_rate);
    } else {
        baud_number = baud_numbers[i];
    }
    */
    
    
    struct termios termios;
    if (tcgetattr(fd, &termios) < 0) {
        fprintf(stderr, "WARNING: tcgetattr failed: %s\n", strerror(errno));
    } else {
        cfmakeraw(&termios);
        termios.c_cflag |= CLOCAL;
        termios.c_cflag &= ~CRTSCTS;
        cfsetispeed(&termios, baud_number);
        cfsetospeed(&termios, baud_number);
        
        if (tcsetattr(fd, TCSANOW, &termios) < 0) {
            fprintf(stderr, "WARNING: tcsetattr failed: %s\n", strerror(errno));
        }
    }
    
    int bits;
    if (ioctl(fd, TIOCMGET, &bits)) {
        fprintf(stderr, "WARNING: ioctl TIOCMGET failed: %s\n", strerror(errno));
    } else {
        bits |= TIOCM_DTR | TIOCM_RTS;
    
        if (ioctl(fd, TIOCMSET, &bits)) {
            fprintf(stderr, "WARNING: ioctl TIOCMSET failed: %s\n", strerror(errno));
        }
    }
    
    return 1;
}

int send_packet(int fd, unsigned char *data, unsigned int data_length) {
	unsigned int packet_length = PREAMBLE_LEN + UARTSYNC_LEN + 1 + data_length;
	unsigned char *send_buffer = (unsigned char *)malloc(packet_length);
	
	memset(send_buffer, PREAMBLE_BYTE, PREAMBLE_LEN);
	memset(send_buffer + PREAMBLE_LEN, UARTSYNC_BYTE, UARTSYNC_LEN);
        send_buffer[PREAMBLE_LEN + UARTSYNC_LEN] = 0;
	memcpy(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + 1, data, data_length);
	
	int n = 0;
	while (n < packet_length) {
		int e = write(fd, send_buffer+n, packet_length - n);
		if (e == -1) {
			fprintf(stderr, "WARNING: send_packet: %s\n", strerror(errno));
		} else {
			n += e;
		}
	}
	free(send_buffer);
        
	return 1;
}

int recv_packet(int fd, unsigned char *data, unsigned int data_length) {
    unsigned char c;
    int e;
    
    do {
        e = read(fd, &c, 1);
        if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet sync1: %s\n", strerror(errno));
        }
    } while (c != UARTSYNC_BYTE);
    
    do {
        e = read(fd, &c, 1);
        if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet sync2: %s\n", strerror(errno));
        }
    } while (c != 0x00);
    
    int n = 0;
    while (n < data_length) {
        e = read(fd, data+n, data_length-n);
        if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet read: %s\n", strerror(errno));
        } else {
            n += e;
        }
    }
        
    return n;
}

unsigned char *test_ber(int fd_tx, int fd_rx, unsigned char *data, unsigned int data_length) {
    struct pollfd pollfds[2] = {
        { fd_rx, POLLIN, 0 } ,
        { fd_tx, POLLOUT, 0 }
    };
    
    int n_sent = 0;
    int n_recv = 0;
    unsigned char *recv = (unsigned char *)malloc(data_length);
    
    fprintf(stderr, "Testing BER with %d bytes\n%s\n", data_length, data);
    
    int timeout = 0;
    while (timeout < 10 && (n_sent < data_length || n_recv < data_length)) {
        poll(pollfds, (n_sent < data_length)?2:1, 1000);
        if (n_recv < data_length) {
            if (pollfds[0].revents & POLLIN) {
                int e = read(fd_rx, recv + n_recv, data_length - n_recv);
                if (e>0) n_recv += e;
                fprintf(stderr, "R%d", n_recv);
            } else {
                timeout++;
                fprintf(stderr, "T%d", timeout);
            }
        }
        if (n_sent < data_length && pollfds[1].revents & POLLOUT) {
            int e = write(fd_tx, data + n_sent, data_length - n_sent);
            if (e>0) n_sent += e;
            fprintf(stderr, "W%d", n_sent);   
        }
    }
    fprintf(stderr, "Received %d bytes\n%s\n", n_recv, recv);
    return recv;
}