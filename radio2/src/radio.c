// $Id: radio.c,v 1.1 2009-10-14 11:39:38 nick Exp $

#include <stdio.h>
#include <stdlib.h>

#include "vtime.h"
#include "node.h"
#include "queue.h"

node_t Node;
queue_t *Queue;

void radio_callback(node_t *node, vtime_t vtime, packet_t *packet) {

    if (packet) {
        
        sendrecv_send(packet);
    
    } else {
        queue_event_t e;
        e.vtime = sim_timer_delay(vtime);
        e.node = node;
        e.packet = NULL;
        queue_insert(Queue, e);
    }
}

int main(int argc, char **argv) {
    
    if (argc < 2) {
        fprintf(stderr, "usage: %s <device> <id> [<timeout>]\n", argv[0]);
        exit(1);
    }
    
    node_init(&Node, atoi(argv[2]));    
    
    // Initialize the queue.  If there's a timeout specified, tell the queue
    // to ignore events after this time.
    Queue = queue_new();
    if (argc >= 4) {
        vtime_t timeout = vtime_add_s(vtime_zero(), atoi(argv[3]));
        queue_set_eschaton(Queue, timeout);
    }
    
    // Let the nodes know how to send packets and request timers.
    node_register_callback(radio_callback);

    // Send Node #0 a timer event to kick things off.
    if (Node.id == 0) {
        queue_event_t epoch_event = { VTIME_ZERO, &Node, NULL };
        queue_insert(Queue, epoch_event);
    }
    
    queue_free(Queue);
}

