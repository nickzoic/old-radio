/* $Id: queue.h,v 1.1 2009-04-08 07:06:51 nick Exp $ */


void queue_register_recv(void (*func)(unsigned char *, unsigned int));
void queue_register_time(void (*func)(), unsigned int timeout);

void queue_send_packet(unsigned char *data, unsigned int length);

void queue_start(int fd);
void queue_stop();
