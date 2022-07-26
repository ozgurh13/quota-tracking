# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>

# include "proc.h"
# include "msleep.h"
# include "open_device.h"

proc_t*
proc_new(const char* device, int mode)
{
	proc_t *proc = malloc(sizeof(proc_t));
	proc->device = calloc(strlen(device) + 1, sizeof(char));
	strcpy(proc->device, device);
	proc->mode = mode;
	proc->active = true;
	proc->next = NULL;

	pthread_create(&proc->thread, NULL, open_device, proc);

	return proc;
}

static inline void
proc_free(proc_t *proc)
{
	free(proc->device);
	free(proc);
}


/*
 * cancels the process and frees memory
 */
void
proc_delete(proc_t *proc)
{
	if (proc->active)
		pthread_cancel(proc->thread);
	if (proc->dev != NULL)
		pcap_close(proc->dev);
	proc_free(proc);
}


void
proc_print(const proc_t *proc)
{
	printf("Proc #%d { device = %s; mode = %d } %s\n"
	      , proc->pid, proc->device, proc->mode
	      , proc->active ? "active" : "inactive" );
}

