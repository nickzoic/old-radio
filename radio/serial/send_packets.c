/* $Id: send_packets.c,v 1.7 2009-02-04 01:13:44 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>

#include "sendrecv.h"
#include "sendrecv_packets.h"

int main(int argc, char **argv) {

    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> [<baud rate> [<num_packets>]]\n", argv[0]);
        exit(1);
    }

    int serial_fd = open(argv[1], O_RDWR);
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    int num_packets = (argc>3)?atoi(argv[3]):baud_rate / 200;

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
    unsigned char packet_data[num_packets][PACKET_LENGTH];
    for (i=0; i<num_packets; i++) {
        if (!fread(packet_data[i], PACKET_LENGTH, 1, data_file)) {
            fprintf(stderr, "couldn't read %s: %s", DATA_FILENAME, strerror(errno));
            exit(4);
        }
    }
    
    int j;
    
    initialize_port(serial_fd, baud_rate);
    for (j=0; j<num_packets; j++) {
        printf ("Sending packet %d / %d\n", j, num_packets);
        send_packet(serial_fd, packet_data[j], PACKET_LENGTH);
        sleep(1);
    }
    return 0;
}
