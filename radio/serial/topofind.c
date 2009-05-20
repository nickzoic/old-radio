// $Id: topofind.c,v 1.1 2009-05-20 08:01:56 nick Exp $

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
#include "crc.h"
#include "beacon.h"

int Interrupted = 0;

void handle_int(int x) {
    Interrupted = 1;
}

#define TIMEOUT (2500)
#define HOLDOFFMAX (1500)
#define HOLDOFFMIN (500)

int main(int argc, char **argv) {
    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> [<baud rate> [<identifier>]]\n", argv[0]);
        exit(1);
    }

    char *device = (argc>1)?argv[1]:"/dev/ttyUSB0";
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    int identifier = (argc>3)?atoi(argv[3]):(int)getpid();

    
    // seed the PRNG from some randomish stuff ...
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec * getpid());
 
    signal(SIGINT, handle_int);
    
    int fd = open(device, O_RDWR | O_SYNC);
    initialize_port(fd, baud_rate);

    unsigned char buffer[1024];
    
    while (!Interrupted) {
        struct timeval tv1, tv2;
        gettimeofday(&tv1, NULL);
        long int timeout = TIMEOUT;
        do {
            int n = 0;
            if (wait_packet(fd, timeout)) {
                n = recv_packet(fd, buffer, sizeof(buffer), 100);
            
                if (n > 2 && buffer[0] == 0xFF && crc16_check(buffer, n)) {
                    printf("%d %d\n", identifier, buffer[2] * 256 + buffer[1]);
                }
            }
            
            gettimeofday(&tv2, NULL);
            long int elapsed = (tv2.tv_sec - tv1.tv_sec) * 1000L + (tv2.tv_usec - tv1.tv_usec) / 1000L;
            timeout = TIMEOUT - elapsed;
            if (n > 0 && timeout < HOLDOFFMAX) {
                long int holdoff = HOLDOFFMIN + (rand() % (HOLDOFFMAX-HOLDOFFMIN));
                if (timeout < holdoff) timeout = holdoff;
            }
        } while(!Interrupted && timeout > 0);
        
        buffer[0] = 0xFF;
	buffer[1] = (identifier % 256);
	buffer[2] = (identifier / 256);
        crc16_set(buffer, 5);
        send_packet(fd, buffer, 5);
    }
}
