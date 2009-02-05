/* $Id: tchatter.c,v 1.1 2009-02-05 01:49:11 nick Exp $ */

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

#define TIMEOUT (5000)
#define HOLDOFFMAX (1000)
#define HOLDOFFMIN (200)

int fd;
pthread_mutex_t transmit_mutex;

void *writer(void *x) {
    unsigned char buffer[1024];
    struct timeval tv1;
    int count = 0;
    
    printf("Writer Started ...\n");
    
    while (1) {
        
        pthread_mutex_lock(&transmit_mutex);
        int n = sprintf((char *)buffer, "Hello %d from %s", count, (char *)x);    
        gettimeofday(&tv1, NULL);
        printf("%03ld.%06ld Say [%s]\n", tv1.tv_sec % 1000, tv1.tv_usec, buffer);
        send_packet(fd, buffer, n);
        pthread_mutex_unlock(&transmit_mutex);
        
        sleep(5);
        count++;
    }
}

void *reader(void *x) {
    struct timeval tv2;
    unsigned char buffer[1024];
    
    printf("Reader Started ...\n");
    
    while (1) {
        int n = recv_packet(fd, buffer, sizeof(buffer)-3, 5000);
        if ( n <= 0 ) continue;
        
        buffer[n] = 0;
            
        gettimeofday(&tv2, NULL);
        printf("%03ld.%06ld Got [%s] %s\n", tv2.tv_sec % 1000, tv2.tv_usec, buffer, (char *)x);    
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
    
    fd = open(device, O_RDWR | O_SYNC);
    initialize_port(fd, baud_rate);
    
    pthread_t reader_thread, writer_thread;
    
    pthread_create(&reader_thread, NULL, reader, device);
    pthread_create(&writer_thread, NULL, writer, device);
    
    sleep(27);
    
    pthread_cancel(reader_thread);
    pthread_cancel(writer_thread);
    
    printf("Fin.\n");
}