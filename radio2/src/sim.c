#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "loc.h"
#include "vtime.h"
#include "topo.h"
#include "node.h"
#include "queue.h"

///////////////////////////////////////////////////////////////////////  GLOBALS

queue_t *Queue;
node_t *Nodes;
int N_nodes;
topo_t *Topo;
vtime_t Eschaton = VTIME_INF;

#define SIM_PROP_DELAY (1000 * VTIME_MICROS)
#define SIM_PROP_DELAY_PERBYTE (1000 * VTIME_MICROS)
#define SIM_PROP_DELAY_MIN (1000 * VTIME_MICROS)
#define SIM_PROP_DELAY_FUZZ (1000 * VTIME_MICROS)
#define SIM_TIMER_DELAY_FUZZ (1000 * VTIME_MICROS)

////////////////////////////////////////////////////////////////  sim_route_test

int sim_route_test_one(node_id_t node1, node_id_t node2, int maxstrat) {
    node_id_t hop = node1;
    for (int i=1; i<100; i++) {
	hop = node_route_mfr(&Nodes[hop], node2, Nodes[node2].virtloc.loc, maxstrat);
	if (hop == node2) { return i; }
	if (hop == NODE_ID_INVALID) { return 0; }
    }
    return 0;
}

void sim_route_test(vtime_t vtime) {

    int success1 = 2;
    int success2 = 2;

    for (int i=1; i < N_nodes; i++) {
	int h11 = sim_route_test_one(i, 0, 1);
	int h12 = sim_route_test_one(0, i, 1);
	if (h11) success1++;
	if (h12) success1++;
	int h21 = sim_route_test_one(i, 0, 2);
	int h22 = sim_route_test_one(0, i, 2);
	if (h21) success2++;
	if (h22) success2++;

    	printf(VTIME_FORMAT " %d Route %d %d  %d %d\n", vtime, i, h11, h12, h21, h22);
    }
    printf(VTIME_FORMAT " 0 Route_Test %f %f\n", vtime,
	 (double)success1 / N_nodes / 2, (double)success2 / N_nodes / 2);
}

////////////////////////////////////////////////////////////////  sim_prop_delay

vtime_t sim_prop_delay(vtime_t vtime, packet_t *packet) {
    long delay = SIM_PROP_DELAY + SIM_PROP_DELAY_PERBYTE * packet->length;
    delay += rand() % SIM_PROP_DELAY_FUZZ;
    if (delay < SIM_PROP_DELAY_MIN) delay = SIM_PROP_DELAY_MIN;
    return vtime + delay;
}

///////////////////////////////////////////////////////////////  sim_timer_delay

vtime_t sim_timer_delay(vtime_t vtime) {
    long delay = rand() % SIM_TIMER_DELAY_FUZZ;
    return vtime + delay * VTIME_MICROS;    
}

//////////////////////////////////////////////////////////////////  sim_callback

void sim_callback(node_t *node, vtime_t vtime, packet_t *packet) {

    if (packet) {
        topo_iter_t *topo_iter = topo_iter_new(Topo, node->id);
        
        topo_entry_t *t;
        while ((t = topo_iter_next(topo_iter))) {
            queue_event_t e;
            e.vtime = sim_prop_delay(vtime, packet);
            if (e.vtime < Eschaton) {
                e.node = &Nodes[t->dst];
                e.packet = packet_clone(packet);
                queue_insert(Queue, e);
            }
        }
        topo_iter_free(topo_iter);
    } else {
        queue_event_t e;
        e.vtime = sim_timer_delay(vtime);
        if (e.vtime < Eschaton) {
            e.node = node;
            e.packet = NULL;
            queue_insert(Queue, e);
        }
    }
}

//////////////////////////////////////////////////////////////////////////  MAIN

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "usage: %s <topofile> [<timeout>]\n", argv[0]);
        exit(1);
    }
    
    srand(vtime_from_wall() * getpid());
    
    // Load the topology file
    Topo = topo_new();
    FILE *fp = fopen(argv[1], "r");
    topo_file_read(Topo, fp);
    fclose(fp);
    
    // Initialize the queue.
    Queue = queue_new();
    
    if (argc >= 3) {
        Eschaton = atoi(argv[2]) * VTIME_SECONDS;
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
    node_set_status(&Nodes[0], VTIME_ZERO, NODE_STATUS_ROOT);
    queue_event_t epoch_event = { VTIME_ZERO, &Nodes[0], NULL };
    queue_insert(Queue, epoch_event);
    
    // while the queue isn't empty, keep on popping
    queue_event_t e;
    vtime_t vtime_tick = VTIME_ZERO;
    while ((e = queue_pop(Queue)).vtime != VTIME_INF) {
        if (e.vtime > vtime_tick) {
            fprintf(stderr, ".");
	    sim_route_test(vtime_tick);
            vtime_tick += VTIME_SECONDS;
        }
        if (e.packet) {
            node_receive(e.node, e.vtime, e.packet);
            packet_free(e.packet);
        } else {
            node_timer(e.node, e.vtime);
        }
    }

    for (int i=0; i < N_nodes; i++) {
        node_deinit(&Nodes[i]);    
    }
    
    fprintf(stderr,"\n");
    
    free(Nodes);
    topo_free(Topo);    
    queue_free(Queue);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
