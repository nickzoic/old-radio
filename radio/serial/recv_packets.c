/* $Id: recv_packets.c,v 1.10 2009-02-05 01:48:17 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include "sendrecv.h"
#include "sendrecv_packets.h"

int Interrupted = 0;

void handle_int(int x) {
    Interrupted = 1;
}

int main(int argc, char **argv) {

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

    signal(SIGINT, handle_int);
 
    unsigned char buffer[PACKET_LENGTH+1] = {0};
    
    initialize_port(serial_fd, baud_rate);
    
    int packet_num = 0;
    int valid_packets = 0;
    
    while(!Interrupted && packet_num < num_packets + 1) {
        printf ("Listening ...\n");
        int n = recv_packet(serial_fd, buffer, PACKET_LENGTH, 1000);
        if (n <= 0) {
            printf("\tTIMEOUT\n");
            if (packet_num) packet_num++;
        } else {
            printf("\tGot %d bytes\n", n);
            if (n == PACKET_LENGTH) {
                for (int j=0; j<num_packets; j++) {
                    int e = 0;
                    for (int k=0; k<PACKET_LENGTH; k++) {
                        if (packet_data[j][k] == buffer[k]) e++;
                    }
                    if (e > PACKET_LENGTH/2) {
                        printf("\t\tmatches packet %d %6.2f%%\n", j, e*100.0/PACKET_LENGTH);
                        packet_num = j;
                        valid_packets++;
                    }
                }
            }
        }
    }
    
    printf("VALID PACKETS: %d/%d %6.2f%%\n", valid_packets, num_packets, valid_packets*100.0/num_packets);
    return num_packets - valid_packets;    
}
