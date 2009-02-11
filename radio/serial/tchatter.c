/* $Id: tchatter.c,v 1.2 2009-02-11 00:55:10 nick Exp $ */

// Chattering with primitive CSMA/CA: pthreads version

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

#define TIMEOUT (5000)
#define HOLDOFFMAX (1000)
#define HOLDOFFMIN (200)

int fd;
pthread_mutex_t collision_mutex;

void *writer(void *x) {
    unsigned char buffer[1024];
    struct timeval tv1, tv2;
    int count = 0;
    
    printf("Writer Started ...\n");
    
    while (1) {
        int n = sprintf((char *)buffer, "Hello %d from %s", count, (char *)x);    
        
        gettimeofday(&tv1, NULL);
        printf("%03ld.%06ld %s Say [%s]\n", tv1.tv_sec % 1000, tv1.tv_usec, (char *)x, buffer);
        
        pthread_mutex_lock(&collision_mutex);
        
        gettimeofday(&tv2, NULL);
        long int delay = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
        printf("%03ld.%06ld %s Delayed %ld\n", tv2.tv_sec % 1000, tv2.tv_usec, (char *)x, delay);
        
        crc16_set(buffer, n+2);
        send_packet(fd, buffer, n+2);
        pthread_mutex_unlock(&collision_mutex);
        
        sleep(5);
        count++;
    }
}

void *reader(void *x) {
    struct timeval tv2;
    unsigned char buffer[1024];
    int holdoff = 0;
    printf("Reader Started ...\n");
    
    while (1) {
        printf("%03ld.%06ld %s Waiting\n", tv2.tv_sec % 1000, tv2.tv_usec, (char *)x);
        
        if (wait_packet(fd, 5000)) {
            pthread_mutex_lock(&collision_mutex);
            do {
                int n = recv_packet(fd, buffer, sizeof(buffer)-3, 100);
                gettimeofday(&tv2, NULL);
                if (crc16_check(buffer,n)) {
                    buffer[n-2] = 0;
                    printf("%03ld.%06ld %s Got [%s]\n", tv2.tv_sec % 1000, tv2.tv_usec, (char *)x, buffer);
                } else {
                    printf("%03ld.%06ld %s Got BAD CRC\n", tv2.tv_sec % 1000, tv2.tv_usec, (char *)x);
                }
                holdoff = HOLDOFFMIN + (rand() % (HOLDOFFMAX-HOLDOFFMIN));
                printf("%03ld.%06ld %s Holdoff %d\n", tv2.tv_sec % 1000, tv2.tv_usec, (char *)x, holdoff);
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
    // int identifier = (argc>3)?atoi(argv[3]):(int)getpid();
    
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