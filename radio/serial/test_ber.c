/* $Id: test_ber.c,v 1.2 2009-01-14 00:11:54 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "sendrecv.h"

unsigned char testdata[10240];

int main(int argc, char **argv) {

    if (argc<3) {
        fprintf(stderr, "usage: %s <txdevname> <rxdevname>\n", argv[0]);
        exit(1);
    }

    int fd_tx = open(argv[1], O_WRONLY | O_NONBLOCK);
    int fd_rx = open(argv[2], O_RDONLY | O_NONBLOCK);
    
    if (fd_tx < 0 || fd_rx < 0) {
        fprintf(stderr, "couldn't open device: %s", strerror(errno));
        exit(1);
    }
    
    FILE *fp = fopen("test_ber.data", "rb");
    fread(testdata, 1, sizeof(testdata), fp);
    fclose(fp);
 
    int baud_rate = 2400;
    initialize_port(fd_rx, baud_rate);
    initialize_port(fd_tx, baud_rate);
    
    test_ber(fd_tx, fd_rx, testdata, sizeof(testdata));
    
        
    return 0;
}
