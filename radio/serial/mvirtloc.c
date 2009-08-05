// $Id: mvirtloc.c,v 1.1 2009-08-05 06:18:30 nick Exp $

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
pthread_mutex_t beacon_mutex;

void *writer(void *x) {
    unsigned char buffer[1024];
    buffer[0] = 0x01;
    
    printf("Writer Started ...\n");
    
    while (1) {
    
        pthread_mutex_lock(&beacon_mutex);        
        beacon_recalc();
        int n = beacon_prepare(buffer+1, sizeof(buffer)-1);
        pthread_mutex_unlock(&beacon_mutex);
        
        crc16_set(buffer, n+3);
        
        pthread_mutex_lock(&collision_mutex);    
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
                    pthread_mutex_lock(&beacon_mutex);
                    beacon_recv(buffer+1, n-3);
                    pthread_mutex_unlock(&beacon_mutex);
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
        fprintf(stderr, "usage: %s <baud rate> <devname*>\n", argv[0]);
        exit(1);
    }

    int baud_rate = atoi(argv[1]);

    for (int i=2; i<argc; i++) {
        char *device = argv[i];
        int identifier = strncmp("/dev/radio", device, 10) ? i - 2 : atoi(argv[i]+10);
        
        printf ("%s %d\n", device, identifier);
    }
}

/*
void foo(int argc, char **argv) {
    int baud_rate = (argc>2)?atoi(argv[2]):9600;
    int identifier = (argc>3)?atoi(argv[3]):(int)getpid();

    // seed the PRNG from some randomish stuff ...
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec * getpid());
    
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
 */