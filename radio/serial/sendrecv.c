/* $Id: sendrecv.c,v 1.20 2009-03-04 07:14:40 nick Exp $ */

// This handles the lowest level of the protocol stack: encoding bytes into
// symbols and pushing them out the serial port.  It doesn't do much in the
// way of consistency checking.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <poll.h>

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "sendrecv.h"
#include "symbols.h"

#define PREAMBLE_LEN (3)
#define UARTSYNC_LEN (2)
#define EPILOGUE_LEN (3)

#define MAX_PACKET_BYTES (8192)
#define MAX_PACKET_SYMBOLS ((MAX_PACKET_BYTES*8/7)+PREAMBLE_LEN+UARTSYNC_LEN+EPILOGUE_LEN+1)

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

int send_packet(int fd, unsigned char *data, unsigned int data_length) {
    // DOESN'T HANDLE BUFFER OVER-RUNS.
    assert(data_length <= MAX_PACKET_BYTES);
    static unsigned char send_buffer[MAX_PACKET_SYMBOLS];
    
    // Create packet w/ Preamble, Sync, Data, Epilogue
    memset(send_buffer, Symbol_Start, PREAMBLE_LEN);
    memset(send_buffer + PREAMBLE_LEN, Symbol_Sync, UARTSYNC_LEN);
    int nsym = bytes_to_symbols(data, data_length, send_buffer+PREAMBLE_LEN+UARTSYNC_LEN);
    memset(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + nsym, Symbol_End, EPILOGUE_LEN);
    
    // Send packet.
    unsigned int packet_length = PREAMBLE_LEN + UARTSYNC_LEN + nsym + EPILOGUE_LEN;
    int n = 0;
    while (n < packet_length) {
        int e = write(fd, send_buffer+n, packet_length - n);
        if (e == -1) {
            fprintf(stderr, "WARNING: send_packet write: %s\n", strerror(errno));
            return 0;
        }
        n += e;    
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////

int wait_packet(int fd, unsigned int timeout) {

   unsigned char c;
    int e;
    
    struct pollfd pollfds[] = {{
	fd, POLLIN, 0    
    }};
    
    // read symbols until we find a Symbol_Start or we timeout.
    
    do {
	e = poll(pollfds, 1, timeout);
	if (e <= 0) {
	    if (e == -1 ) fprintf(stderr, "WARNING: recv_packet sync poll: %s\n", strerror(errno));
	    return 0;
	}
    
        e = read(fd, &c, 1);
	if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet sync read: %s\n", strerror(errno));
	    return 0;
        } 
    } while (c != Symbol_Start);
    
    return 1;
}

#define RECV_BUFFER_LEN (MAX_PACKET_SYMBOLS)

int recv_packet(int fd, unsigned char *data, unsigned int data_length, unsigned int timeout) {

    // read symbols until we find a Symbol_End or we run out of buffer.
 
    static unsigned char recv_buffer[RECV_BUFFER_LEN];
    
    struct pollfd pollfds[] = {{
	fd, POLLIN, 0    
    }};
    
    int n = 0;
    int stopped = 0;
    while (!stopped && n < RECV_BUFFER_LEN) {
	
        int e = poll(pollfds, 1, timeout);
	if (e <= 0) {
	    if (e == -1) fprintf(stderr, "WARNING: recv_packet read poll: %s\n", strerror(errno));
	    return 0;
	}
        
        e = read(fd, recv_buffer+n, RECV_BUFFER_LEN-n);
	if (e == -1) {
            fprintf(stderr, "WARNING: recv_packet read read: %s\n", strerror(errno));
	    return 0;
        }
        
        for (int i=n; i<n+e; i++) {
            if (recv_buffer[i] == Symbol_End) {
                stopped = 1;
                e = i-n;
                break;
            }
        }
        n += e;
    }
    if (!stopped) {
        fprintf(stderr, "WARNING: recv_packet overrun\n");
        return 0;
    }
    
    // Find offset of first valid symbol
    int i;
    for (i=0; i<n; i++) {
        if (symbol_valid(recv_buffer[i])) break;
    }
    
    // Decode packet starting from first valid symbol
    int m = symbols_to_bytes(recv_buffer+i, n-i, data);    
    return m;
}

////////////////////////////////////////////////////////////////////////////////

void flush_packet(int fd) {
    int e;
    char c;
    
    struct pollfd pollfds[] = {{
	fd, POLLIN, 0    
    }};
    
    while(1) {
	e = poll(pollfds, 1, 100);
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
