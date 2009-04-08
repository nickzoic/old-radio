// $Id: queue.c,v 1.3 2009-04-08 08:49:20 nick Exp $

// Run a packet queue in its own thread.  This thread handles tranmitting
// and receiving packets.  This code also handles CSMA/CA.

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/types.h>

#include "sendrecv.h"
#include "queue.h"

struct queue_s {
    unsigned char *data;
    unsigned int length;
    struct queue_s *next;
};
typedef struct queue_s queue_t;

// This mutex is used to prevent the send thread trying to send while the recv
// thread is receiving.

pthread_mutex_t queue_holdoff_mutex;
int queue_holdoff_min = 100;
int queue_holdoff_max = 500;

// Send thread stuff ... this thread maintains a queue of packets to send, and
// handles sending them.

pthread_t queue_send_thread;
pthread_mutex_t queue_send_mutex;
pthread_cond_t queue_send_cv;
queue_t *queue_send_head = NULL;
queue_t *queue_send_tail = NULL;

// Recv thread stuff ... this thread just waits for packets to come in
// and calls the recv_handler and/or calls the time_handler for timeouts.

pthread_t queue_recv_thread;
void (*queue_recv_handler)(unsigned char *, unsigned int);

//------------------------------------------------------------------------------
// queue_register_recv: set the receiver handlers

void queue_register_recv(void (*func)(unsigned char *, unsigned int))
{
    queue_recv_handler = func;
}

//------------------------------------------------------------------------------
// queue_set_holdoff: set the holdoff parameters
// queue_rand_holdoff: pick a (randomized) holdoff time.

void queue_set_holdoff(unsigned int holdoff_min, unsigned int holdoff_max)
{
    queue_holdoff_min = holdoff_min;
    queue_holdoff_max = holdoff_max;
}

unsigned int queue_rand_holdoff()
{
    return queue_holdoff_min + (rand() % (queue_holdoff_max - queue_holdoff_min));
}

//------------------------------------------------------------------------------
// queue_send_packet: add a packet to the send queue.

void queue_send_packet(unsigned char *data, unsigned int length)
{
    // Allocate a new queue entry and copy the data.
    unsigned char *qdata = (unsigned char *)malloc(length);
    memcpy(qdata, data, length);
    queue_t *nqueue = (queue_t *)malloc(sizeof(queue_t));
    nqueue->data = qdata;
    nqueue->length = length;
    nqueue->next = NULL;
    
    // Add it to the tail of the queue (in a mutex)
    pthread_mutex_lock(&queue_send_mutex);
        if (!queue_send_tail) {
            // if queue is empty, create new queue and wake up sender thread.
            queue_send_tail = queue_send_head = nqueue;
            pthread_cond_signal(&queue_send_cv);
        } else {
            // otherwise, just add packet to end of queue.
            queue_send_tail->next = nqueue;
            queue_send_tail = nqueue;
        }
    pthread_mutex_unlock(&queue_send_mutex);
}

//------------------------------------------------------------------------------
// queue_send_get_size: measures the queue length by iterating down along the
// queue (not terribly efficient, but I didn't want to add a queue_length.

int queue_send_get_size()
{
    if (!queue_send_head) return 0;
    
    int count = 0;
    pthread_mutex_lock(&queue_send_mutex);
    
        queue_t *x = queue_send_head;
        while (x) {
            x = x->next;
            count++;
        }
        
    pthread_mutex_unlock(&queue_send_mutex);
    
    return count;
}

//------------------------------------------------------------------------------
// queue_sender: runs in the queue_send_thread

void *queue_sender(void *xfd)
{
    int fd = (int)xfd;
    
    while(1) {
        
        // Lock the queue_send_mutex, waiting while there's no packets to send,
        pthread_mutex_lock(&queue_send_mutex);
            while (!queue_send_head) {
                pthread_cond_wait(&queue_send_cv, &queue_send_mutex);
            }
        
            // pop the next packet to send off the head of the queue and unlock.
            queue_t *xqueue = queue_send_head;
            queue_send_head = xqueue->next;
            if (!queue_send_head) queue_send_tail = NULL;
        pthread_mutex_unlock(&queue_send_mutex);
            
        // Lock the queue_holdoff_mutex and send the packet.
        pthread_mutex_lock(&queue_holdoff_mutex);
        
            send_packet(fd, xqueue->data, xqueue->length);
        
        pthread_mutex_unlock(&queue_holdoff_mutex);
        
        // Finally, deallocate queue buffer.
        free(xqueue->data);
        free(xqueue);
    }
}

//------------------------------------------------------------------------------
// queue_receiver: runs in the queue_recv_thread

void *queue_receiver(void *xfd)
{
    int fd = (int)xfd;
    unsigned char buffer[1024];
    
    while (1) {
        if (wait_packet(fd, 5000)) {
            pthread_mutex_lock(&queue_holdoff_mutex);
            
                do {
                    int nrecv = recv_packet(fd, buffer, sizeof(buffer)-3, 100);
                    if (nrecv && queue_recv_handler) queue_recv_handler(buffer, nrecv);           
                } while (wait_packet(fd, queue_rand_holdoff()));
                
            pthread_mutex_unlock(&queue_holdoff_mutex);
        }
    } 
}

//------------------------------------------------------------------------------
// queue_start and queue_stop: start and stop the threads, and associated
// mutexes

void queue_start(int fd)
{
    pthread_mutex_init(&queue_holdoff_mutex, NULL);
    pthread_mutex_init(&queue_send_mutex, NULL);
    pthread_cond_init(&queue_send_cv, NULL);

    pthread_create(&queue_send_thread, NULL, queue_sender, (void *)fd);
    pthread_create(&queue_recv_thread, NULL, queue_receiver, (void *)fd);
}

void queue_stop()
{
    pthread_cancel(queue_send_thread);
    pthread_cancel(queue_recv_thread);
    
    pthread_mutex_destroy(&queue_holdoff_mutex);
    pthread_mutex_destroy(&queue_send_mutex);
    pthread_cond_destroy(&queue_send_cv);
}

//==============================================================================