/* $Id: sendrecv.c,v 1.12 2009-02-04 01:13:44 nick Exp $ */

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
#include "symbols.h"

#define PREAMBLE_LEN (5)
#define UARTSYNC_LEN (2)
#define EPILOGUE_LEN (5)

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
    // XXX TOFIX: Doesn't check for buffer overruns
    static unsigned char send_buffer[8192];
    static unsigned char flush_buffer[64];
    
    struct pollfd pollfds[] = {{
	fd, POLLIN|POLLOUT, 0    
    }};
    
    memset(send_buffer, Symbol_Start, PREAMBLE_LEN);
    memset(send_buffer + PREAMBLE_LEN, Symbol_Sync, UARTSYNC_LEN);
    int nsym = bytes_to_symbols(data, data_length, send_buffer+PREAMBLE_LEN+UARTSYNC_LEN);
    memset(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + nsym, Symbol_End, EPILOGUE_LEN);
    unsigned int packet_length = PREAMBLE_LEN + UARTSYNC_LEN + nsym + EPILOGUE_LEN;
    
    int n = 0;
    while (n < packet_length) {
        int e = poll(pollfds, 1, 1000);
        if (e == -1) {
            fprintf(stderr, "WARNING: send_packet poll: %s\n", strerror(errno));
	    return 0;
        } else {
            if (pollfds[0].revents & POLLIN) {
                // While we're transmitting, our own receiver keeps copping a lot
                // of random nonsense.
                e = read(fd, flush_buffer, sizeof(flush_buffer));
                if (e == -1) {
                    fprintf(stderr, "WARNING: send_packet read: %s\n", strerror(errno));
                }
            }
            if (pollfds[0].revents & POLLOUT) {
                e = write(fd, send_buffer+n, packet_length - n);
                if (e == -1) {
                    fprintf(stderr, "WARNING: send_packet write: %s\n", strerror(errno));
                } else {
                    n += e;
                }    
            }
        }
    }
    flush_packet(fd);
    
    return 1;
}

#define RECV_BUFFER_LEN (8192)

int recv_packet(int fd, unsigned char *data, unsigned int data_length, unsigned int timeout) {
    unsigned char c;
    int e;
    
    static unsigned char recv_buffer[RECV_BUFFER_LEN];
    
    struct pollfd pollfds[] = {{
	fd, POLLIN, 0    
    }};
    
    do {
	e = poll(pollfds, 1, timeout);
	if (e == -1) {
	    fprintf(stderr, "WARNING: recv_packet sync poll: %s\n", strerror(errno));
	    return 0;
	} else if (e == 0) {
	    return 0;
	}
    
        e = read(fd, &c, 1);
	if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet sync read: %s\n", strerror(errno));
	    return 0;
        } 
    } while (c != Symbol_Start);
    
    int n = 0;
    int stopped = 0;
    while (!stopped && n < RECV_BUFFER_LEN) {
	
	e = poll(pollfds, 1, 1000);
	if (e == -1) {
	    fprintf(stderr, "WARNING: recv_packet read poll: %s\n", strerror(errno));
	    return n;
	} else if (e == 0) {
	    fprintf(stderr, "WARNING: recv_packet read poll timeout\n");
	    return n;
	}
    
        e = read(fd, recv_buffer+n, RECV_BUFFER_LEN-n);
	if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet read read: %s\n", strerror(errno));
	    return n;
        } else {
            for (int i=n; i<n+e; i++) {
                if (recv_buffer[i] == Symbol_End) {
                    stopped = 1;
                    e = i-n;
                    break;
                }
            }
            n += e;
        }
    }
    if (!stopped) return 0;
    
    // Find offset of first valid symbol
    int i;
    for (i=0; i<n; i++) {
        if (symbol_valid(recv_buffer[i])) break;
    }
    
    // Find number of invalid symbols
    //e = 0;
    //for (int j=i; j<n; j++) {
    //    if (!symbol_valid(recv_buffer[j])) e++;
    //}
    
    // Decode packet starting from first valid symbol
    int m = symbols_to_bytes(recv_buffer+i, n-i, data);
    
    //fprintf(stderr, "recv_packet: got symbols=%d bytes=%d offset=%d invalid=%d\n", n, m, i, e);
    return m;
}

void flush_packet(int fd) {
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
            fprintf(stderr, "WARNING: flush_packet read: %s\n", strerror(errno));
	    return;
        } 
    };
}
