/* $Id: chatter.c,v 1.4 2009-02-05 01:48:51 nick Exp $ */

// Chattering with primitive CSMA/CA

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
#include <sys/types.h>

#include "sendrecv.h"

int Interrupted = 0;

void handle_int(int x) {
    Interrupted = 1;
}

#define TIMEOUT (5000)
#define HOLDOFFMAX (1000)
#define HOLDOFFMIN (200)

int main(int argc, char **argv) {
    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> [<baud rate> [<identifier>]]\n", argv[0]);
        exit(1);
    }

    char *device = (argc>1)?argv[1]:"/dev/ttyUSB0";
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    int identifier = (argc>3)?atoi(argv[3]):(int)getpid();
 
    signal(SIGINT, handle_int);
    
    int fd = open(device, O_RDWR | O_SYNC);
    initialize_port(fd, baud_rate);
    
    unsigned char buffer[1024];
    int count = 0;
    
    while (!Interrupted) {
        struct timeval tv1, tv2;
        gettimeofday(&tv1, NULL);
        long int timeout = TIMEOUT;
        printf("%03ld.%06ld Listen %ld\n", tv1.tv_sec % 1000, tv1.tv_usec, timeout);
        do {
            
            
            int n = recv_packet(fd, buffer, sizeof(buffer)-3, timeout);
            
            if (n > 0) {
                // do something!
                buffer[n] = 0;
                
                gettimeofday(&tv2, NULL);
                printf("%03ld.%06ld Got [%s]\n", tv2.tv_sec % 1000, tv2.tv_usec, buffer);
            }
            
            gettimeofday(&tv2, NULL);
            long int elapsed = (tv2.tv_sec - tv1.tv_sec) * 1000L + (tv2.tv_usec - tv1.tv_usec) / 1000L;
            timeout = TIMEOUT - elapsed;
            if (n > 0 && timeout < HOLDOFFMAX) {
                printf("%03ld.%06ld Holdoff %ld\n", tv2.tv_sec % 1000, tv2.tv_usec, timeout);
                long int holdoff = HOLDOFFMIN + (rand() % (HOLDOFFMAX-HOLDOFFMIN));
                if (timeout < holdoff) timeout = holdoff;
            }
            printf("%03ld.%06ld Listen2 %ld\n", tv2.tv_sec % 1000, tv2.tv_usec, timeout);
        } while(!Interrupted && timeout > 0);
        
        
        count++;
        int n = sprintf((char *)buffer, "Hello %d from %d", count, identifier);
        printf("%03ld.%06ld Say [%s]\n", tv2.tv_sec % 1000, tv2.tv_usec, buffer);
        send_packet(fd, buffer, n);
    
    }
    
    printf("Fin.\n");
}