# include <stdio.h>
# include <string.h>

# include "proclist.h"


/*
 * process list
 *
 * the list is a circular linked list with a ptr to the tail
 * implementation of variant d from
 *     https://web.archive.org/web/20160731005714/http://brpreiss.com/books/opus8/html/page96.html
 *
 *
 *      +------------------------------------------------------------------------------+
 *      |                                                                              |
 *      v                                                                              |
 *    +---+---+        +---+---+        +---+---+        +---+---+        +---+---+    |
 *    |   | --+------> |   | --+------> |   | --+------> |   | --+------> |   | --+----+
 *    +---+---+        +---+---+        +---+---+        +---+---+        +---+---+
 *                                                                          ^
 *                                                                          |
 * list_ptr +---------------------------------------------------------------+
 *
 *
 * this way we can append to the end of the list in (1), without having to traverse
 * the entire list or keep an extra ptr to the tail
 *
 * delete operations are still O(n) as the element can be anywhere in the list
 */


static bool
list_isempty(proclist_t *list)
{
	return list->size == 0;
}


void
proclist_init(proclist_t *list)
{
	list->tail = NULL;
	list->size = 0;
	list->n = 0;
}


static void
proclist_copy(proclist_t *dstlist, proclist_t *srclist)
{
	dstlist->tail = srclist->tail;
	dstlist->size = srclist->size;
	dstlist->n    = srclist->n;
}


static void
list_prepend(proclist_t *list, proc_t *proc)
{
	if (list_isempty(list)) {
		list->tail = proc;
		proc->next = proc;
	}
	else {
		proc_t *head = list->tail->next;
		list->tail->next = proc;
		proc->next = head;
	}
	list->size++;
}


bool
proclist_add(proclist_t *list, proc_t *proc)
{
	// assign pid to proc
	proc->pid = list->n;
	list->n++;

	list_prepend(list, proc);
	list->tail = list->tail->next;

	return true;
}


/*
 * pops the last element of the list
 * shouldn't be called on empty lists
 *
 * this is used when the element we have
 * a ptr to in the list (list->tail)
 * needs to deleted
 *
 * complexity: O(n)
 */
static proc_t*
list_poptail(proclist_t *list)
{
	/*  find node before tail  */
	proc_t *node, *tail = list->tail;
	for ( node = tail
	    ; node->next != tail
	    ; node = node->next
	    ) /*  do nothing  */;

	/*  single element list  */
	if (node == tail)
		list->tail = NULL;
	else {
		node->next = tail->next;
		list->tail = node;
	}

	return tail;
}


bool
proclist_deleteby_device(proclist_t *list, const char* device)
{
	if (list_isempty(list))
		return false;

	proc_t *delete = NULL;
	if (strcmp(list->tail->device, device) == 0)       // delete tail
		delete = list_poptail(list);
	else {
		const proc_t *tail = list->tail;
		proc_t *p = list->tail;
		while (p->next != tail && strcmp(p->next->device, device) != 0)
			p = p->next;

		if (p->next == tail)
			return false;

		proc_t *d = p->next;
		p->next = d->next;
		delete = d;
	}

	if (delete != NULL) {
		proc_delete(delete);
		list->size--;

		return true;
	}

	return false;
}


bool
proclist_deleteby_id(proclist_t *list, const int pid)
{
	if (list_isempty(list))
		return false;

	proc_t *delete = NULL;
	if (list->tail->pid == pid)       // delete tail
		delete = list_poptail(list);
	else {
		const proc_t *tail = list->tail;
		proc_t *p = list->tail;
		while (p->next != tail && p->next->pid != pid)
			p = p->next;

		if (p->next == tail)
			return false;

		proc_t *d = p->next;
		p->next = d->next;
		delete = d;
	}

	if (delete != NULL) {
		proc_delete(delete);
		list->size--;

		return true;
	}

	return false;
}


static proc_t*
list_pophead(proclist_t *list)
{
	proc_t *head = list->tail->next;

	/*  single element list  */
	if (list->tail == head)
		list->tail = NULL;
	else
		list->tail->next = head->next;

	return head;
}


/*
 * delete all running processes
 */
bool
proclist_delete_all(proclist_t *list)
{
	while (!list_isempty(list)) {
		proc_delete(list_pophead(list));
		list->size--;
	}

	return true;
}


/*
 * remove all inactive processes
 */
size_t
proclist_delete_inactive(proclist_t *list)
{
	size_t count = 0;      // number of inactive processes found

	if (list_isempty(list))
		goto done;

	proclist_t dlist;
	proclist_copy(&dlist, list);

	const size_t size = list->size;
	for (size_t i = 0; i < size; i++) {      // cycle thru dlist
		if (!dlist.tail->next->active) {
			proc_delete(list_pophead(&dlist));
			dlist.size--;
			count++;
		}
		else
			dlist.tail = dlist.tail->next;
	}

	proclist_copy(list, &dlist);

done:
	return count;
}


void
proclist_print(proclist_t *list)
{
	printf(":: procs (%zu) ::\n", list->size);

	if (list_isempty(list))
		return;

	proc_t *tail = list->tail, *proc = tail;

	do {
		proc = proc->next;
		printf(":> ");
		proc_print(proc);
	} while (proc != tail);
}

void
proclist_print_inactive(proclist_t *list)
{
	puts(":: inactive procs ::");
	size_t count = 0;

	if (list_isempty(list))
		goto done;

	proc_t *tail = list->tail, *proc = tail;

	do {
		proc = proc->next;
		if (!proc->active) {
			count++;
			printf(":> ");
			proc_print(proc);
		}
	} while (proc != tail);

done:
	printf(" < count %zu >\n", count);
}

