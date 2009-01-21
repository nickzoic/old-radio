/* $Id: sendrecv.c,v 1.7 2009-01-21 12:38:16 nick Exp $ */

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

#define PREAMBLE_LEN (10)
#define PREAMBLE_BYTE (0x55)
#define UARTSYNC_LEN (1)
#define UARTSYNC_BYTE (0xFF)
#define SLACK_LEN (20)
#define SLACK_BYTE (0xAA)

int baud_table[][2] = {
    { B38400, 38400 },
    { B19200, 19200 },
    { B9600, 9600 },
    { B4800, 4800 },
    { B2400, 2400 },
    { B1200, 1200 },
    { B600, 600 },
    { B300, 300 },
    { 0, 0 },
};

#define BITMASK(b) ((1 << (b)) - 1)
#define GETBITS(x,a,b) (((x) >> (a)) & BITMASK(b))

char preamble[] = "UUUUUUUUUU\xFF\xFF\x00";

#include "symbols.h"
#define PREAMBLE SYMBOL_RESERVED_0
#deinfe ENDFRAME SYMBOL_RESERVED_1

int bytes_to_symbols(unsigned char *bytes, int nbytes, unsigned char *symbols) {
    symbols[0] = symbols_encode[                           GETBITS(bytes[0], 0, 7)      ];
    if (nbytes < 2) return 1;
    symbols[1] = symbols_encode[ GETBITS(bytes[0], 7, 1) + GETBITS(bytes[1], 0, 6) << 1 ];
    if (nbytes < 3) return 2;
    symbols[2] = symbols_encode[ GETBITS(bytes[1], 6, 2) + GETBITS(bytes[2], 0, 5) << 2 ];
    if (nbytes < 4) return 3;
    symbols[3] = symbols_encode[ GETBITS(bytes[2], 5, 3) + GETBITS(bytes[3], 0, 4) << 3 ];
    if (nbytes < 5) return 4;
    symbols[4] = symbols_encode[ GETBITS(bytes[3], 4, 4) + GETBITS(bytes[4], 0, 3) << 4 ];
    if (nbytes < 6) return 5;
    symbols[5] = symbols_encode[ GETBITS(bytes[4], 3, 5) + GETBITS(bytes[5], 0, 2) << 5 ];
    if (nbytes < 7) return 6;
    symbols[6] = symbols_encode[ GETBITS(bytes[5], 2, 6) + GETBITS(bytes[6], 0, 1) << 6 ];
    if (nbytes < 8) return 7;
    symbols[7] = symbols_encode[ GETBITS(bytes[6], 1, 7)                                ];
    return 8;
}

int symbols_to_bytes(unsigned char *symbols, unsigned char *bytes) {
    
    unsigned char s0 = symbols_decode[symbols[0]];
    unsigned char s1 = symbols_decode[symbols[1]];
    if (s0 == -1 || s1 == -1) return 0;
    bytes[0] = GETBITS(s0, 0, 7) + GETBITS(s1, 0, 1);
    
    unsigned char s2 = symbols_decode[symbols[2]];
    if (s2 == -1) return 1;
    bytes[1] = GETBITS(s1, 1, 6) + GETBITS(s2, 0, 2);
    
    unsigned char s3 = symbols_decode[symbols[3]];
    if (s3 == -1) return 2;
    bytes[2] = GETBITS(s2, 2, 5) + GETBITS(s3, 0, 3);
    
    unsigned char s4 = symbols_decode[symbols[4]];
    if (s4 == -1) return 3;
    bytes[3] = GETBITS(s3, 3, 4) + GETBITS(s4, 0, 4);
    
    unsigned char s5 = symbols_decode[symbols[5]];
    if (s5 == -1) return 4;
    bytes[4] = GETBITS(s4, 4, 3) + GETBITS(s5, 0, 5);
    
    unsigned char s6 = symbols_decode[symbols[6]];
    if (s6 == -1) return 5;
    bytes[5] = GETBITS(s5, 5, 2) + GETBITS(s6, 0, 6);
    
    unsigned char s7 = symbols_decode[symbols[7]];
    if (s7 == -1) return 6;
    bytes[6] = GETBITS(s6, 6, 1) + GETBITS(s7, 0, 7);

    return 7;
}

int initialize_port(int fd, int baud_rate) {

    int i;
    for (i=0; baud_table[i][1]; i++) {
	if (baud_rate == baud_table[i][1])
		baud_rate = baud_table[i][0];
    }
    
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
	unsigned int packet_length = PREAMBLE_LEN + UARTSYNC_LEN + 1 + data_length + SLACK_LEN;
	unsigned char *send_buffer = (unsigned char *)calloc(packet_length, 1);
	
	memset(send_buffer, PREAMBLE_BYTE, PREAMBLE_LEN);
	memset(send_buffer + PREAMBLE_LEN, UARTSYNC_BYTE, UARTSYNC_LEN);
        send_buffer[PREAMBLE_LEN + UARTSYNC_LEN] = 0;
	memcpy(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + 1, data, data_length);
	memset(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + 1 + data_length, SLACK_BYTE, SLACK_LEN);

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
    
    struct pollfd pollfds[] = {{
	fd, POLLIN, 0    
    }};
    
    do {
	e = poll(pollfds, 1, 1000);
	if (e == -1) {
	    fprintf(stderr, "WARNING: recv_packet sync1 poll: %s\n", strerror(errno));
	    return 0;
	} else if (e == 0) {
	    fprintf(stderr, "WARNING: recv_packet sync1 poll timeout\n");
	    return 0;
	}
    
        e = read(fd, &c, 1);
	if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet sync1 read: %s\n", strerror(errno));
	    return 0;
        } 
    } while (c != UARTSYNC_BYTE);
    
    do {
	
	e = poll(pollfds, 1, 1000);
	if (e == -1) {
	    fprintf(stderr, "WARNING: recv_packet sync2 poll: %s\n", strerror(errno));
	    return 0;
	} else if (e == 0) {
	    fprintf(stderr, "WARNING: recv_packet sync2 poll timeout\n");
	    return 0;
	}
    
        e = read(fd, &c, 1);
        if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet sync2: %s\n", strerror(errno));
	    return 0;
	}
    } while (c == UARTSYNC_BYTE);
    
    int n = 0;
    while (n < data_length) {
	
	e = poll(pollfds, 1, 1000);
	if (e == -1) {
	    fprintf(stderr, "WARNING: recv_packet read poll: %s\n", strerror(errno));
	    return n;
	} else if (e == 0) {
	    fprintf(stderr, "WARNING: recv_packet read poll timeout\n");
	    return n;
	}
    
        e = read(fd, data+n, data_length-n);
	if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet read read: %s\n", strerror(errno));
	    return n;
        } else {
            n += e;
        }
    }
        
    return n;
}

void flush_packet(fd) {
    int e;
    char c;
    
    struct pollfd pollfds[] = {{
	fd, POLLIN, 0    
    }};
    
    while(1) {
	e = poll(pollfds, 1, 1000);
	if (e == -1) {
	    fprintf(stderr, "WARNING: flush_packet poll: %s\n", strerror(errno));
	    return;
	} else if (e == 0) {
	    /* fprintf(stderr, "WARNING: flush_packet poll timeout\n"); */
	    return;
	}
    
        e = read(fd, &c, 1);
	if (e == -1) {
            fprintf(stderr, "WARNING: flush_packet ead: %s\n", strerror(errno));
	    return;
        } 
    };
}
