#include <stdio.h>
#include <stdlib.h>

#include "vtime.h"
#include "topo.h"
#include "node.h"
#include "queue.h"

#define XMIT_DELAY_MS (1)

queue_t *Queue;
node_t *Nodes;
int N_nodes;
topo_t *Topo;

#define SIM_PROP_DELAY_US (1000)
#define SIM_PROP_DELAY_PERBYTE_US (1000)
#define SIM_PROP_DELAY_MIN_US (1000)
#define SIM_PROP_DELAY_FUZZ_US (1000)

#define SIM_TIMER_DELAY_FUZZ_US (1000)

vtime_t sim_prop_delay(vtime_t vtime, packet_t *packet) {
    long delay_us = SIM_PROP_DELAY_US + SIM_PROP_DELAY_PERBYTE_US * packet->length;
    delay_us += rand() % SIM_PROP_DELAY_FUZZ_US;
    if (delay_us < SIM_PROP_DELAY_MIN_US) delay_us = SIM_PROP_DELAY_MIN_US;
    return vtime + delay_us * VTIME_MICROS;
}

vtime_t sim_timer_delay(vtime_t vtime) {
    long delay_us = rand() % SIM_TIMER_DELAY_FUZZ_US;
    return vtime + delay_us * VTIME_MICROS;    
}

void sim_callback(node_t *node, vtime_t vtime, packet_t *packet) {

    if (packet) {
        topo_iter_t *topo_iter = topo_iter_new(Topo, node->id);
    
        topo_entry_t *t;
        while ((t = topo_iter_next(topo_iter))) {
            queue_event_t e;
            e.vtime = sim_prop_delay(vtime, packet);
            e.node = &Nodes[t->dst];
            e.packet = packet_clone(packet);
            queue_insert(Queue, e);
        }
        
        topo_iter_free(topo_iter);
    } else {
        queue_event_t e;
        e.vtime = sim_timer_delay(vtime);
        e.node = node;
        e.packet = NULL;
        queue_insert(Queue, e);
    }
}


int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "usage: %s <topofile> [<timeout>]\n", argv[0]);
        exit(1);
    }
    
    // Load the topology file
    Topo = topo_new();
    FILE *fp = fopen(argv[1], "r");
    topo_file_read(Topo, fp);
    fclose(fp);
    
    // Initialize the queue.  If there's a timeout specified, tell the queue
    // to ignore events after this time.
    Queue = queue_new();
    if (argc >= 3) {
        vtime_t timeout = atoi(argv[2]) * VTIME_SECONDS;
        queue_set_eschaton(Queue, timeout);
    }
    
    // allocate node table & initialize all the nodes
    N_nodes = topo_max_id(Topo)+1;
    Nodes = (node_t *)calloc(N_nodes, sizeof(node_t));
    for (int i=0; i < N_nodes; i++) {
        node_init(&Nodes[i], i);    
    }
    
    // Let the nodes know how to send packets and request timers.
    node_register_callback(sim_callback);
        
    // Send Node #0 a timer event to kick things off.
    queue_event_t epoch_event = { VTIME_ZERO, &Nodes[0], NULL };
    queue_insert(Queue, epoch_event);
    
    // while the queue isn't empty, keep on popping
    queue_event_t e;
    while ((e = queue_pop(Queue)).vtime != VTIME_INF) {
        if (e.packet) {
            node_receive(e.node, e.vtime, e.packet);
            packet_free(e.packet);
        } else {
            node_timer(e.node, e.vtime);
        }
    }

    free(Nodes);
    topo_free(Topo);    
    queue_free(Queue);
    return 0;
}