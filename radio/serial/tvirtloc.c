// $Id: tvirtloc.c,v 1.1 2009-02-11 08:28:06 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/types.h>

#include "sendrecv.h"
#include "crc.h"
#include "beacon.h"

#define TIMEOUT (5000)
#define HOLDOFFMAX (1000)
#define HOLDOFFMIN (200)

int fd;
pthread_mutex_t collision_mutex;

void *writer(void *x) {
    unsigned char buffer[1024];
    
    printf("Writer Started ...\n");
    
    while (1) {
        pthread_mutex_lock(&collision_mutex);
        
        buffer[0] = 0x01;
        int n = beacon_prepare(buffer+1, sizeof(buffer)-1);        
        crc16_set(buffer, n+3);
        send_packet(fd, buffer, n+3);
        
        pthread_mutex_unlock(&collision_mutex);
        sleep(5);
    }
}

void *reader(void *x) {
    unsigned char buffer[1024];
    int holdoff = 0;
    printf("Reader Started ...\n");
    
    while (1) {
        if (wait_packet(fd, 5000)) {
            pthread_mutex_lock(&collision_mutex);
            
            do {
                int n = recv_packet(fd, buffer, sizeof(buffer)-3, 100);
                if (crc16_check(buffer,n) && buffer[0] == 0x01) {
                    beacon_recv(buffer+1, n-3);
                } else {
                    printf("BAD PACKET %d %d\n", n, buffer[0]);
                }
                holdoff = HOLDOFFMIN + (rand() % (HOLDOFFMAX-HOLDOFFMIN));
            } while (wait_packet(fd, holdoff));
            
            pthread_mutex_unlock(&collision_mutex);
        }
    }
}

int main(int argc, char **argv) {
    if (argc<2) {
        fprintf(stderr, "usage: %s <devname> [<baud rate> [<identifier>]]\n", argv[0]);
        exit(1);
    }

    char *device = (argc>1)?argv[1]:"/dev/ttyUSB0";
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    int identifier = (argc>3)?atoi(argv[3]):(int)getpid();
    
    beacon_init(identifier);
    
    fd = open(device, O_RDWR | O_SYNC);
    initialize_port(fd, baud_rate);
    
    pthread_t reader_thread, writer_thread;
    
    pthread_create(&reader_thread, NULL, reader, device);
    pthread_create(&writer_thread, NULL, writer, device);
    
    sleep(270);
    
    pthread_cancel(reader_thread);
    pthread_cancel(writer_thread);
    
    printf("Fin.\n");
}