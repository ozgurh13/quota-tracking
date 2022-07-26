#ifndef __FIFO_H__
#define __FIFO_H__

# define  FIFO_BUFFER_SIZE    1024

static const char* fifofile = "/tmp/quota.fifo";

void fifo_listen();

#endif
