/* $Id: sendrecv.c,v 1.3 2009-01-14 03:02:22 nick Exp $ */

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

int baud_table[][2] = {
    /* { B38400, 38400 },
    { B19200, 19200 }, */
    { B9600, 9600 },
    { B4800, 4800 },
    { B2400, 2400 },
    
    { 0, 0 },
};

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
