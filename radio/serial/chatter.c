/* $Id: chatter.c,v 1.1 2009-02-04 01:13:44 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include <sys/types.h>

#include "sendrecv.h"

int Interrupted = 0;

void handle_int(int x) {
    Interrupted = 1;
}

int main(int argc, char **argv) {
    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> [<baud rate> [<identifier>]]\n", argv[0]);
        exit(1);
    }

    char *device = (argc>1)?argv[1]:"/dev/ttyUSB0";
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    int identifier = (argc>3)?atoi(argv[3]):(int)getpid();
    
    int fd = open(device, O_RDWR | O_SYNC);
    initialize_port(fd, baud_rate);
    
    unsigned char buffer[1024];
    int count = 0;
    
    while (!Interrupted) {
        unsigned int timeout = 200 + rand() % 800;
        printf("Listen %d\n", timeout);
        int n = recv_packet(fd, buffer, sizeof(buffer)-1, timeout);
        if (n > 0) {
            buffer[n] = 0;
            printf("\t\tGot [%s]\n", buffer);
        } else {
            count++;
            int n = sprintf((char *)buffer, "Hello %d from %d", count, identifier);
            printf("\tSay [%s]\n", buffer);
            send_packet(fd, buffer, n);
        }
    }
}