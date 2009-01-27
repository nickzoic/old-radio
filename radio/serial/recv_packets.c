/* $Id: recv_packets.c,v 1.4 2009-01-27 23:59:18 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>

#include "sendrecv.h"
#include "sendrecv_packets.h"

int bit_compare(unsigned char *a, unsigned char *b, unsigned int len) {
    int i, r=0;
    for (i=0; i<len; i++) {
        unsigned char d = a[i] ^ b[i];
        while (d) {
            d &= d - 1;
            r++;
        }
    }
    return r;
}

int main(int argc, char **argv) {

    if (argc<2) {
        fprintf(stderr, "usage: %s <devname>\n", argv[0]);
        exit(1);
    }

    int serial_fd = open(argv[1], O_RDWR);
    int baud_rate = (argc>2)?atoi(argv[2]):2400;

    if (serial_fd < 0) {
        fprintf(stderr, "couldn't open %s: %s", argv[1], strerror(errno));
        exit(2);
    }
    
    FILE *data_file = fopen(DATA_FILENAME, "rb");
    if (!data_file) {
        fprintf(stderr, "couldn't open %s: %s", DATA_FILENAME, strerror(errno));
        exit(3);
    }
    int i;
    unsigned char packet_data[NUM_PACKETS][PACKET_LENGTH];
    for (i=0; i<NUM_PACKETS; i++) {
        if (!fread(packet_data[i], PACKET_LENGTH, 1, data_file)) {
            fprintf(stderr, "couldn't read %s: %s", DATA_FILENAME, strerror(errno));
            exit(4);
        }
    }

    int j, k;
    unsigned char buffer[PACKET_LENGTH] = {0};
    
    initialize_port(serial_fd, baud_rate);
    for (j=0; j<NUM_PACKETS; j++) {
        printf ("\tPacket %d ...", j);
	while(1) {
        	int n = recv_packet(serial_fd, buffer, PACKET_LENGTH);
		if (n>0) break;
		printf(" TIMEOUT\n\tPacket %d ...", j);
	}
        
        int bits = bit_compare(buffer, packet_data[j], PACKET_LENGTH);
        printf ("Bit Errors: %d\n", bits);
        
        if (bits && bits < 100) {
            for (k=0; k<PACKET_LENGTH; k++) {
                fprintf(stderr,"%d %02X %02X\n", k, buffer[k], packet_data[j][k]);
            }
        }
        
        sleep(1);
    }
    return 0;
    
}
