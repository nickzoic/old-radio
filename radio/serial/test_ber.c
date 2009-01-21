/* $Id: test_ber.c,v 1.5 2009-01-21 07:22:50 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "sendrecv.h"

int BaudRates[] = { 9600, 4800, 2400, 1200, 0 };

unsigned char testdata[256];
unsigned char recvdata[256];

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

void alarmhandler(int a) {
    fprintf(stderr, "GOT ALARM\n");
}

int main(int argc, char **argv) {

    
    if (argc<3) {
        fprintf(stderr, "usage: %s <txdevname> <rxdevname>\n", argv[0]);
        exit(1);
    }

    int fd_tx = open(argv[1], O_WRONLY );
    int fd_rx = open(argv[2], O_RDONLY );
    
    if (fd_tx < 0 || fd_rx < 0) {
        fprintf(stderr, "couldn't open device: %s", strerror(errno));
        exit(1);
    }
    
    FILE *fp = fopen("test_ber.data", "rb");
    if (fread(testdata, 1, sizeof(testdata), fp) != sizeof(testdata)) {
        fprintf(stderr, "couldn't read test_ber.data: %s", strerror(errno));
        exit(2);
    }
    fclose(fp);
    /* memset(testdata, 0xAA, sizeof(testdata)); */
 
    int i, j, k;
    
    for (i=0; BaudRates[i]; i++) {
        int baud_rate = BaudRates[i];

        initialize_port(fd_rx, baud_rate);
        initialize_port(fd_tx, baud_rate);
    
        for (j=0; j < 10; j++) {
        
            flush_packet(fd_rx);
            
            pid_t childpid = fork();
            if (childpid == 0) {
                /* the child */
                usleep(10000);
                send_packet(fd_tx, testdata, sizeof(testdata));
                usleep(100000);
                exit(0);
            }
            int nread = recv_packet(fd_rx, recvdata, sizeof(recvdata));
            wait(NULL);
            
            
            int errors = bit_compare(testdata, recvdata, sizeof(testdata));
        
            printf("baud=%6d test=%2d nread=%d errors=%d ber=%f\n", baud_rate, j, nread, errors, (double)errors/(sizeof(testdata)*8));
    
            if (errors < 100) {
                for (k=0; k < nread; k++) {
                    if (testdata[k] != recvdata[k])
                        printf("%6d %02X %02X\n", k, testdata[k], recvdata[k]);
                }
            }
        }
    }
    
    return 0;
}
