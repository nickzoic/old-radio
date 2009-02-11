// $Id: spy.c,v 1.4 2009-02-11 23:21:42 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

#include <sys/time.h>

#include "sendrecv.h"
#include "symbols.h"

int main(int argc, char **argv) {

    if (argc<2) {
	fprintf(stderr, "usage: %s <device> [<baud_rate>]\n", argv[0]);
	exit(1);
    }

    int fd = open(argv[1], O_RDWR);
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    
    if (fd < 0) {
        fprintf(stderr, "couldn't open %s: %s\n", argv[1], strerror(errno));
        exit(2);
    }
    
    initialize_port(fd, baud_rate);
    
    int nsym = 0;
    int valid = 0;
    unsigned char symbols[10];
    unsigned char bytes[10];
    
    while(1) {
        unsigned char x = 0;
        int n = read(fd, &x, 1);
        if (n != 1) continue;
        
        int x_valid = symbol_valid(x);
        if ((nsym == 8) || (x_valid && !valid) || (!x_valid && valid)) {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            
            printf("%03d.%03d > ", (int)(tv.tv_sec % 1000), (int)(tv.tv_usec / 1000));
            for (int i=0; i<nsym; i++) {
                printf("%02X ", symbols[i]);
            }
            if (valid) {
                int m = symbols_to_bytes(symbols, nsym, bytes);
                printf("%*s| ", (8-nsym)*3, "");
                for (int i=0; i<m; i++) {
                    printf("%02X ", bytes[i]);
                }
		printf("%*s| ", (7-m)*3, "");
		for (int i=0; i<m; i++) {
		    printf("%c", isprint(bytes[i])?bytes[i]:'.');
		}
            }
            printf ("\n");
            nsym = 0;
            valid = x_valid;
        }
        symbols[nsym++] = x;
    }
}

