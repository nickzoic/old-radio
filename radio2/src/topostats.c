// $Id: topostats.c,v 1.1 2009-11-17 03:51:43 nick Exp $

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/types.h>

#include "vtime.h"
#include "radio.h"

///////////////////////////////////////////////////////////////////////  GLOBALS

#define BEACON_INIT_DELAY (10 * VTIME_SECONDS)
#define BEACON_PERIOD_MIN (750 * VTIME_MILLIS)
#define BEACON_PERIOD_MAX (1500 * VTIME_MILLIS)

#define MAXID (255)

typedef struct beacon_s {
    int id;
    int serial;
    char gibberish[53];
} beacon_t;

//////////////////////////////////////////////////////////////////////////  MAIN

int main(int argc, char **argv) {
    
    if (argc < 4) {
        fprintf(stderr, "usage: %s <device> <id> <npackets>\n", argv[0]);
        exit(1);
    }
    
    radio_t *radio = radio_new(argv[1], B9600);
    int id = atoi(argv[2]);
    int npackets = atoi(argv[3]);
    
    // seed the PRNG from some randomish stuff ...
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec * getpid() * id);
    
    vtime_t vtime = vtime_from_wall();
    vtime_t eschaton = vtime + (BEACON_INIT_DELAY * 2) + (BEACON_PERIOD_MAX * npackets);
    vtime_t next_time = vtime + BEACON_INIT_DELAY;
    
    int serial = 0;
    int counter[MAXID+1] = { 0 };
    
    while(vtime < eschaton) {
	
	while (vtime < next_time && vtime < eschaton) {
	    vtime_t timeout = (next_time < eschaton) ? next_time : eschaton;
	    if (radio_wait(radio, timeout)) {
		packet_t *packet = radio_recv(radio, timeout);
		if (packet) {
		    beacon_t *b = (beacon_t *)packet->data;
		    vtime = vtime_from_wall();
		    //printf(VTIME_FORMAT_STR " %d %d %d %d\n", VTIME_FORMAT_ARG(vtime), id, packet->length, b->id, b->serial);
		    if (b->id <= MAXID) counter[b->id]++;
		    packet_free(packet);
		}
            }
	    vtime = vtime_from_wall();
	}
	if (serial < npackets) {
	    beacon_t beacon = { id, serial, "gibberish" };
	    packet_t *packet = packet_new(sizeof(beacon_t), &beacon);
	    radio_send(radio, packet);
	    packet_free(packet);
	    next_time += ((BEACON_PERIOD_MAX - BEACON_PERIOD_MIN) * (float)rand() / RAND_MAX) + BEACON_PERIOD_MIN;
	    serial++;
	} else {
	    next_time = VTIME_INF;
	}
	vtime = vtime_from_wall();
    }	

    radio_free(radio);
    
    for (int i=0; i<=MAXID; i++) {
	if (i != id && counter[i]) {
	    printf("%d %d %f\n", id, i, (float)counter[i]/npackets);
	}
    }
}

////////////////////////////////////////////////////////////////////////////////