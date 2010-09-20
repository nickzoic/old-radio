// $Id: serial.c,v 1.2 2010-02-01 10:31:04 nick Exp $

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
#include "node.h"
#include "radio.h"

///////////////////////////////////////////////////////////////////////  GLOBALS

#define SERIAL_MTU (1024)
#define SERIAL_MAX_FRAME (SERIAL_MTU * 8 / 7 + 8)

node_t *Node;
radio_t *Radio;

// this should be replaced by an event queue just like in the simulator.
// instead of using Timer, use queue_top(Queue).vtime as the time of the
// next event.
vtime_t Timer = VTIME_INF;
void *Extra = NULL;

///////////////////////////////////////////////////////////////  serial_callback

void serial_callback(node_t *node, vtime_t vtime, packet_t *packet, void *extra) {
    if (packet) {
	radio_send(Radio, packet);
    } else {
	assert(vtime < Timer);
	Timer = vtime;
	Extra = extra;
    }
}

//////////////////////////////////////////////////////////////////////////  MAIN

int main(int argc, char **argv) {
    
    if (argc < 4) {
        fprintf(stderr, "usage: %s <device> <id> <timeout>\n", argv[0]);
        exit(1);
    }

    // seed the PRNG from some randomish stuff ...
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec * getpid());
 
    Node = node_new(atoi(argv[2]), "");    
    
    Radio = radio_new(argv[1], B9600);
    
    // Let the nodes know how to send packets and request timers.
    node_register_callback(serial_callback);

    vtime_t vtime = vtime_from_wall();
    vtime_t eschaton = vtime + atoi(argv[3]) * VTIME_SECONDS;
 
    // Send Node #0 a timer event to kick things off.
    if (Node->id == 0) {
	Timer = vtime;
    }
 
    while(vtime < eschaton) {
	while (vtime < Timer && vtime < eschaton) {
	    vtime_t timeout = (Timer < eschaton) ? Timer : eschaton;
	    if (radio_wait(Radio, timeout)) {
		packet_t *packet = radio_recv(Radio, timeout);
		if (packet) {
		    vtime = vtime_from_wall();
		    node_receive(Node, vtime, packet);
		    packet_free(packet);
		}
            }
	    vtime = vtime_from_wall();
	}
	if (vtime >= Timer) {
	    Timer = VTIME_INF;
	    node_timer(Node, vtime, Extra);
	}
	vtime = vtime_from_wall();
    }	

    node_free(Node);
    radio_free(Radio); 
}

////////////////////////////////////////////////////////////////////////////////
