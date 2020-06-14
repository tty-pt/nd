/* Primitives Package */

#include "copyright.h"
#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "db.h"
#include "inst.h"
#include "externs.h"
#include "match.h"
#include "interface.h"
#include "params.h"
#include "fbstrings.h"
#include "interp.h"
#include "array.h"

struct mufevent_process {
	struct mufevent_process *prev, *next;
	dbref player;
	dbref prog;
	short filtercount;
	short deleted;
	char** filters;
	struct frame *fr;
} *mufevent_processes;

/* static int event_has_refs(dbref program, struct mufevent_process *proc)
 * Checks the MUF event queue for address references on the stack or
 * dbref references on the callstack
 */
static int
event_has_refs(dbref program, struct mufevent_process *proc)
{
	int loop;
	struct frame* fr = NULL;
	
	if (proc->deleted) {
		return 0;
	}
	fr = proc->fr;
	if (!fr) {
		return 0;
	}

	for (loop = 1; loop < fr->caller.top; loop++) {
		if (fr->caller.st[loop] == program) {
			return 1;
		}
	}

	for (loop = 0; loop < fr->argument.top; loop++) {
		if (fr->argument.st[loop].type == PROG_ADD &&
			fr->argument.st[loop].data.addr->progref == program) {
			return 1;
		}
	}

	return 0;
}

/* int muf_event_dequeue(dbref prog, int killmode)
 * Deregisters a program from any instances of it in the EVENT_WAIT queue.
 * killmode values:
 *     0: kill all matching processes (Equivalent to 1)
 *     1: kill all matching MUF processes
 *     2: kill all matching foreground MUF processes
 */
int
muf_event_dequeue(dbref prog, int killmode)
{
	struct mufevent_process *proc;
	int count = 0;

	if(killmode == 0)
		killmode = 1;

	for (proc = mufevent_processes; proc; proc = proc->next) {
		if (proc->deleted) {
			continue;
		}
		if (proc->prog != prog && !event_has_refs(prog, proc) && proc->player != prog) {
			continue;
		}
		if (killmode == 2) {
			if (proc->fr && proc->fr->multitask == BACKGROUND) {
				continue;
			}
		} else if (killmode == 1) {
			if (!proc->fr) {
				continue;
			}
		}
		if (!proc->fr->been_background)
			PLAYER_SET_BLOCK(proc->player, 0);
		muf_event_purge(proc->fr);
		prog_clean(proc->fr);
		proc->deleted = 1;
		count++;
	}
	return count;
}

struct frame*
muf_event_pid_frame(int pid)
{
	struct mufevent_process *ptr = mufevent_processes;

	while (ptr) {
		if (!ptr->deleted && ptr->fr && ptr->fr->pid == pid)
			return ptr->fr;
		ptr = ptr->next;
	}
	return NULL;
}

/* int muf_event_controls(dbref player, int pid)
 * Returns true if the given player controls the given PID.
 */
int
muf_event_controls(dbref player, int pid)
{
	struct mufevent_process *proc = mufevent_processes;

	while (proc && (proc->deleted || pid != proc->fr->pid)) {
		proc = proc->next;
	}

	if (!proc) {
		return 0;
	}
	if (!controls(player, proc->prog) && player != proc->player) {
		return 0;
	}

	return 1;
}

/* static void muf_event_free(struct mufevent* ptr)
 * Frees up a MUF event once you are done with it.  This shouldn't be used
 * outside this module.
 */
static void
muf_event_free(struct mufevent *ptr)
{
	CLEAR(&ptr->data);
	free(ptr->event);
	ptr->event = NULL;
	ptr->next = NULL;
	free(ptr);
}

/* struct mufevent* muf_event_pop_specific(struct frame* fr, int eventcount, const char** events)
 * Removes the first event of one of the specified types from the event queue
 * of the given program instance.
 * Returns a pointer to the removed event to the caller.
 * Returns NULL if no matching events are found.
 * You will need to call muf_event_free() on the returned data when you
 * are done with it and wish to free it from memory.
 */
struct mufevent*
muf_event_pop_specific(struct frame *fr, int eventcount, char **events)
{
	struct mufevent *tmp = NULL;
	struct mufevent *ptr = NULL;
	int i;

	for (i = 0; i < eventcount; i++) {
		if (fr->events && equalstr(events[i], fr->events->event)) {
			tmp = fr->events;
			fr->events = tmp->next;
			return tmp;
		}
	}

	ptr = fr->events;
	while (ptr && ptr->next) {
		for (i = 0; i < eventcount; i++) {
			if (equalstr(events[i], ptr->next->event)) {
				tmp = ptr->next;
				ptr->next = tmp->next;
				return tmp;
			}
		}
		ptr = ptr->next;
	}

	return NULL;
}

/* static struct mufevent* muf_event_peek(struct frame* fr)
 * This returns a pointer to the top muf event of the given
 * program instance's event queue.  The event is not removed
 * from the queue.
 */
/*
 * Commented out by Winged for non-reference.  If you need it, de-comment.
 *
static struct mufevent *
muf_event_peek(struct frame *fr)
{
	return fr->events;
}
 */

/* static struct mufevent* muf_event_pop(struct frame* fr)
 * This pops the top muf event off of the given program instance's
 * event queue, and returns it to the caller.  The caller should
 * call muf_event_free() on the data when it is done with it.
 */
static struct mufevent *
muf_event_pop(struct frame *fr)
{
	struct mufevent *ptr = NULL;

	if (fr->events) {
		ptr = fr->events;
		fr->events = fr->events->next;
	}
	return ptr;
}

/* void muf_event_purge(struct frame* fr)
 * purges all muf events from the given program instance's event queue.
 */
void
muf_event_purge(struct frame *fr)
{
	while (fr->events) {
		muf_event_free(muf_event_pop(fr));
	}
}

static const char *mufevent_c_version = "$RCSfile$ $Revision: 1.31 $";
const char *get_mufevent_c_version(void) { return mufevent_c_version; }
