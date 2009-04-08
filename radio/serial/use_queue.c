// $Id: use_queue.c,v 1.2 2009-04-08 08:38:08 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>
#include <sys/stat.h>

#include "sendrecv.h"
#include "queue.h"

char testpacket[] = "ThisIsATestPackets";

void recv_handler(unsigned char *data, unsigned int length) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    int okay = ((length == sizeof(testpacket)) && !strncmp((char *)data, testpacket, length-5));
    fprintf(stderr, "%06d.%06d recv_handler(%p, %d) -> %d\n", (int)tv.tv_sec, (int)tv.tv_usec, data, length, okay);
}

void time_handler() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    fprintf(stderr, "%06d.%06d time_handler()\n", (int)tv.tv_sec, (int)tv.tv_usec);
    queue_send_packet((unsigned char *)testpacket, sizeof(testpacket));
}

int main(int argc, char **argv) {

    int fd = open(argv[1], O_RDWR | O_SYNC);
    initialize_port(fd, 9600);
    
    queue_register_recv(recv_handler);
    // queue_register_time(time_handler, 100);
    
    queue_start(fd);
    
    for (int i=0; i<10000; i++) {
        usleep(100000);
        struct timeval tv;
        gettimeofday(&tv, NULL);
    
        fprintf(stderr, "%06d.%06d sending %d\n", (int)tv.tv_sec, (int)tv.tv_usec, queue_send_size());
        queue_send_packet((unsigned char *)testpacket, sizeof(testpacket));
    }
    
    queue_stop();
    
    close(fd);
    return 0;
}
