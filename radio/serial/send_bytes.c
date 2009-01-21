/* $Id: send_bytes.c,v 1.1 2009-01-21 07:22:50 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include "sendrecv.h"

int main(int argc, char **argv) {

    if (argc<4) {
        fprintf(stderr, "usage: %s <devname> <baud> <byte>+\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_WRONLY);
    
    if (fd < 0) {
        fprintf(stderr, "couldn't open %s: %s", argv[1], strerror(errno));
        exit(2);
    }
   
    int baud_rate = atoi(argv[2]);
    int buffer_length = argc - 3;

    int i;
    unsigned char *send_buffer = (unsigned char *)malloc(buffer_length);
    for (i=0; i<buffer_length; i++) {
	send_buffer[i] = strtol(argv[i+3], NULL, 0);
    }
    
    printf("Initializing ...\n");

    initialize_port(fd, baud_rate);

    printf("Sending ...");

    for (i=0; i< baud_rate / buffer_length / 10; i++) {
	int nsent = 0;
	while (nsent < buffer_length) {
	    int x = write(fd, send_buffer + nsent, buffer_length - nsent);
	    if (x>0) nsent += x;
	}
    }

    printf("\nExiting ...\n");

    return 0;
}
