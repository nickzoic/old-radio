// $Id: radio.c,v 1.2 2009-10-18 04:34:04 nick Exp $

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
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "radio.h"
#include "symbols.h"

#define PREAMBLE_LEN (3)
#define UARTSYNC_LEN (2)
#define EPILOGUE_LEN (3)

#define MAX_FRAME ( (RADIO_MTU + 6) * 8 / 7 + PREAMBLE_LEN + UARTSYNC_LEN + EPILOGUE_LEN )

////////////////////////////////////////////////////////////////////////////////

radio_t *radio_new(char *devname, int baud_rate) {

    int fd = open(devname, O_RDWR | O_NONBLOCK | O_SYNC);
    assert(fd > 0);

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
    
    radio_t *radio = (radio_t *)malloc(sizeof(radio_t));
    radio->fd = fd;
    return radio;
}

////////////////////////////////////////////////////////////////////////////////

void radio_send(radio_t *radio, packet_t *packet) {
    assert(packet->length <= RADIO_MTU);
    static unsigned char send_buffer[MAX_FRAME];
    
    // Create packet w/ Preamble, Sync, Data, Epilogue
    memset(send_buffer, Symbol_Start, PREAMBLE_LEN);
    memset(send_buffer + PREAMBLE_LEN, Symbol_Sync, UARTSYNC_LEN);
    int nsym = bytes_to_symbols(packet->data, packet->length, send_buffer+PREAMBLE_LEN+UARTSYNC_LEN);
    memset(send_buffer + PREAMBLE_LEN + UARTSYNC_LEN + nsym, Symbol_End, EPILOGUE_LEN);
    
    // Send packet.
    unsigned int packet_length = PREAMBLE_LEN + UARTSYNC_LEN + nsym + EPILOGUE_LEN;
    int n = 0;
    while (n < packet_length) {
        int e = write(radio->fd, send_buffer+n, packet_length - n);
        assert(e>=0);
        n += e;    
    }
}

////////////////////////////////////////////////////////////////////////////////

int radio_wait(radio_t *radio, vtime_t timeout) {

    unsigned char c;
    int e;
    
    struct pollfd pollfds[] = {{
	radio->fd, POLLIN, 0    
    }};
    
    // read symbols until we find a Symbol_Start or we timeout.
    
    do {
        int timeout_ms = (timeout - vtime_from_wall()) / VTIME_MILLIS;
        if (timeout_ms < 1) return 0;
        
	e = poll(pollfds, 1, timeout_ms);
        if (e == 0) return 0;
        assert(e>0);
        
	e = read(radio->fd, &c, 1);
        assert(e==1);
    } while (c != Symbol_Start);
    
    return 1;
}

////////////////////////////////////////////////////////////////////////////////

packet_t *radio_recv(radio_t *radio, vtime_t timeout) {

    static unsigned char recv_buffer[MAX_FRAME];
    static unsigned char packet_buffer[RADIO_MTU];
    
    struct pollfd pollfds[] = {{
	radio->fd, POLLIN, 0    
    }};
    
    int n = 0;
    int stopped = 0;
    while (!stopped && n < MAX_FRAME) {
	
        int timeout_ms = (timeout - vtime_from_wall()) / VTIME_MILLIS;
        if (timeout_ms < 1) return 0;
        
        int e = poll(pollfds, 1, timeout_ms);
        if (e == 0) break;
	assert(e == 1);
        
        e = read(radio->fd, recv_buffer+n, MAX_FRAME-n);
        assert (e>0);
	
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
    int nbytes = symbols_to_bytes(recv_buffer+i, n-i, packet_buffer);
    
    return packet_new(nbytes, packet_buffer);
}

////////////////////////////////////////////////////////////////////////////////

void radio_flush(radio_t *radio) {
    int e;
    char c;
    
    struct pollfd pollfds[] = {{
	radio->fd, POLLIN, 0    
    }};
    
    while(1) {
	e = poll(pollfds, 1, 100);
        if (e == 0) return;
        assert(e == 1);
        
        e = read(radio->fd, &c, 1);
        assert(e == 1);
    };
}

////////////////////////////////////////////////////////////////////////////////

void radio_free(radio_t *radio) {
    close(radio->fd);
    free(radio);
}


