#ifndef __PROC_H__
#define __PROC_H__

# include <stdbool.h>
# include <pthread.h>
# include <pcap.h>

/*
 * process struct
 */
typedef struct proc_t {
	size_t pid;                    // process id
	char* device;                  // device being listened to
	int mode;                      // listening mode

	volatile bool active;          // is the thread active
	pthread_t thread;              // thread id of running process
	pcap_t *dev;                   // capture device

	struct proc_t *next;           // next process
} proc_t;

proc_t* proc_new(const char* device, int mode);
void proc_delete(proc_t *proc);
void proc_print(const proc_t *proc);

#endif
