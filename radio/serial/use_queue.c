// $Id: use_queue.c,v 1.1 2009-04-08 07:06:51 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/stat.h>

#include "sendrecv.h"
#include "queue.h"

char *testpacket = "ThisIsATestPacket";

void recv_handler(unsigned char *data, unsigned int length) {
    
    fprintf(stderr, "recv_handler(%p, %d)\n", data, length);
}

void time_handler() {
    fprintf(stderr, "time_handler()\n");
    queue_send_packet((unsigned char *)testpacket, strlen(testpacket));
}

int main(int argc, char **argv) {

    int fd = open(argv[1], O_RDWR | O_SYNC);
    initialize_port(fd, 9600);
    
    queue_register_recv(recv_handler);
    queue_register_time(time_handler, 1000);
    
    queue_start(fd);
    
    sleep(10000);
    
    queue_stop();
    
    close(fd);
    return 0;
}
