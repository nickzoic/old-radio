/* $Id: send_byte.c,v 1.2 2009-01-21 07:22:50 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include "sendrecv.h"

int Interrupted = 0;

void handle_int(int x) {
    Interrupted = 1;
}

int main(int argc, char **argv) {

    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> <baud> <byte>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_WRONLY);
    
    if (fd < 0) {
        fprintf(stderr, "couldn't open %s: %s", argv[1], strerror(errno));
        exit(2);
    }
   
    int baud_rate = (argc>2)?atoi(argv[2]):2400;
    int byte = (argc>3)?strtol(argv[3], NULL, 0):0x55;

    signal(SIGINT, handle_int);
 
    printf("Initializing ...\n");

    initialize_port(fd, baud_rate);

    unsigned char send_buffer[100];

    memset(send_buffer, byte, sizeof(send_buffer));

    printf("Sending ...");

    int i;
    for (i=0; i< baud_rate / 200; i++) {
	write(fd, send_buffer, sizeof(send_buffer));
    }

    printf("\nExiting ...\n");

    return 0;
}
