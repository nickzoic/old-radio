/* $Id: test_ber.c,v 1.1 2008-12-10 05:44:28 nick Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "sendrecv.h"

unsigned char testdata[] =
       "\x55\x55\x55\x55\x55\x55\x55\xFF\xFF"
       "On success, the number of bytes read is returned (zero indi"
       "cates  end  of  file),  and the file position is advanced by "
       "this number.  It is not an error if this number  is  smaller "
       "than  the  number  of  bytes  requested; this may happen for "
       "example because fewer bytes are actually available right now "
       "(maybe  because  we were close to end-of-file, or because we "
       "are reading from a pipe, or from  a  terminal),  or  because "
       "read()  was  interrupted  by  a  signal.   On  error,  -1 is "
       "returned, and errno is set appropriately.  In this  case  it "
       "is  left  unspecified  whether  the  file  position (if any) "
       "changes.";

int main(int argc, char **argv) {

    if (argc<3) {
        fprintf(stderr, "usage: %s <txdevname> <rxdevname>\n", argv[0]);
        exit(1);
    }

    int fd_tx = open(argv[1], O_WRONLY);
    int fd_rx = open(argv[2], O_RDONLY);
    
    if (fd_tx < 0 || fd_rx < 0) {
        fprintf(stderr, "couldn't open device: %s", strerror(errno));
        exit(1);
    }
    
    int baud_rate = 2400;
    initialize_port(fd_rx, baud_rate);
    initialize_port(fd_tx, baud_rate);
    
    test_ber(fd_tx, fd_rx, testdata, sizeof(testdata));
    
        
    return 0;
}
