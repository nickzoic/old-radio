/* $Id: queue.h,v 1.3 2009-04-08 08:49:20 nick Exp $ */


void queue_register_recv(void (*func)(unsigned char *, unsigned int));
void queue_set_holdoff(unsigned int holdoff_min, unsigned int holdoff_max);

void queue_send_packet(unsigned char *data, unsigned int length);
int queue_send_get_size();

void queue_start(int fd);
void queue_stop();
