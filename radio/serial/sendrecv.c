/* $Id: sendrecv.c,v 1.2 2009-01-14 00:11:54 nick Exp $ */

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

#define PREAMBLE_LEN (50)
#define PREAMBLE_BYTE (0x55)
#define UARTSYNC_LEN (5)
#define UARTSYNC_BYTE (0xFF)

int initialize_port(int fd, int baud_rate) {
    
    struct termios termios;
    if (tcgetattr(fd, &termios) < 0) {
        fprintf(stderr, "WARNING: tcgetattr failed: %s\n", strerror(errno));
    } else {
        cfmakeraw(&termios);
        termios.c_cflag |= CLOCAL;
        termios.c_cflag &= ~CRTSCTS;
        cfsetispeed(&termios, baud_rate);
        cfsetospeed(&termios, baud_rate);
        
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
	/* memcpy(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + 1, "HELLO", 5); */
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
            fprintf(stderr, "sync1: WARNING: recv_packet sync1: %s\n", strerror(errno));
        } else {
	    /* fprintf(stderr, "sync1: %02X\n", c); */
	}
    } while (c != UARTSYNC_BYTE);
    
    do {
        e = read(fd, &c, 1);
        if (e == -1) {
            fprintf(stderr, "sync2: WARNING: recv_packet sync2: %s\n", strerror(errno));
        } else {
	    /* fprintf(stderr, "sync2: %02X\n", c); */
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
	{ fd_tx, POLLOUT, 0 } ,
    };
    
    int n_sent = 0;
    int n_recv = 0;
    int n_print = 0;

    unsigned char *recv = (unsigned char *)malloc(data_length);
    
    fprintf(stderr, "Testing BER with %d bytes\n", data_length);
    
    printf("HELLO!\n");

    while ((n_sent < data_length) || (n_recv < data_length)) {
	pollfds[0].events = (n_recv < data_length)?POLLIN:0;
	pollfds[1].events = (n_sent < data_length)?POLLOUT:0;

	int e = poll(pollfds, 2, 1000);
	if (e == 0) break;

	if (pollfds[0].revents & POLLIN) {
	    if (n_recv == 0) {
		e = read(fd_rx, recv, 1);
		if (e == 1 && recv[0] == data[0]) n_recv++;
	    } else {
	    	e = read(fd_rx, recv + n_recv, data_length - n_recv);
	    	if (e>0) n_recv += e;
	    }
	    printf("R%d\n", e);
	}

	if (pollfds[1].revents & POLLOUT) {
    	    e = write(fd_tx, data + n_sent, data_length - n_sent);
            if (e>0) n_sent += e;
	    printf("W%d\n", e);
	}

	while (n_print < n_recv) {
	    printf ("%6d %02X %02X\n", n_print, data[n_print], recv[n_print]);
	    n_print++;
	}
    }


    fprintf(stderr, "Received %d bytes\n", n_recv);
    return recv;
}
