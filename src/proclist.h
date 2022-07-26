#ifndef __PROCLIST_H__
#define __PROCLIST_H__

# include <stdbool.h>
# include "proc.h"


typedef struct {
	proc_t *tail;          // ptr to tail
	size_t size;           // current size
	size_t n;              // next pid
} proclist_t;


void proclist_init(proclist_t *list);
bool proclist_add(proclist_t *list, proc_t *proc);
bool proclist_deleteby_device(proclist_t *list, const char* device);
bool proclist_deleteby_id(proclist_t *list, const int pid);
bool proclist_delete_all(proclist_t *list);
size_t proclist_delete_inactive(proclist_t *list);
void proclist_print(proclist_t *list);
void proclist_print_inactive(proclist_t *list);

#endif
