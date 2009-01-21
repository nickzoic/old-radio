/* $Id: recv_bytes.c,v 1.1 2009-01-21 07:22:50 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "sendrecv.h"

int Interrupted = 0;

void handle_int(int x) {
    Interrupted = 1;
}

int main(int argc, char **argv) {

    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> <baud>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    
    if (fd < 0) {
        fprintf(stderr, "couldn't open %s: %s", argv[1], strerror(errno));
        exit(2);
    }
   
    int baud_rate = (argc>2)?atoi(argv[2]):2400;

    signal(SIGINT, handle_int);
 
    printf("Initializing ...\n");

    initialize_port(fd, baud_rate);

    printf("Listening ...\n");

    unsigned char recv_buffer[16];

    while (!Interrupted) {
	int n = read(fd, recv_buffer, sizeof(recv_buffer));
	if (n < 0) {
	    fprintf(stderr, "WARNING: read error: %s", strerror(errno));
	} else if ( n > 0) {
	    struct timeval tv;
	    gettimeofday(&tv, NULL);
	    printf("%10d.%06d [%02d] ", (int)tv.tv_sec, (int)tv.tv_usec, n);
	    
	    int i;
 	    for (i=0; i<n; i++) {
		printf ("%02X ", recv_buffer[i]);
	    }
	    printf(".\n");
	}
    }

    printf("\nExiting ...\n");

    return 0;
}
