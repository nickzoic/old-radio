// $Id: spy.c,v 1.1 2009-02-11 08:27:21 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#include "sendrecv.h"
#include "symbols.h"

int main(int argc, char **argv) {

    int fd = open(argv[1], O_RDWR);
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    
    if (fd < 0) {
        fprintf(stderr, "couldn't open %s: %s", argv[1], strerror(errno));
        exit(1);
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
            
            printf("%010ld.%06ld>> ", tv.tv_sec, tv.tv_usec);
            for (int i=0; i<nsym; i++) {
                printf("%02X ", symbols[i]);
            }
            if (valid) {
                int m = symbols_to_bytes(symbols, nsym, bytes);
                printf("%*s| ", (8-nsym)*3, "");
                for (int i=0; i<m; i++) {
                    printf("%02X ", bytes[i]);
                }
            }
            printf ("\n");
            nsym = 0;
            valid = x_valid;
        }
        symbols[nsym++] = x;
    }
}
    