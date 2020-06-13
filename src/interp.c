/* Muf Interpreter and dispatcher. */

#include "copyright.h"
#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "db.h"
#include "inst.h"
#include "externs.h"
#include "match.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "fbstrings.h"
#include "interp.h"
#include "debug.h"

/* This package performs the interpretation of mud forth programs.
   It is a basically push pop kinda thing, but I'm making some stuff
   inline for maximum efficiency.

   Oh yeah, because it is an interpreted language, please do type
   checking during this time.  While you're at it, any objects you
   are referencing should be checked against db_top.
   */

/* in cases of boolean expressions, we do return a value, the stuff
   that's on top of a stack when a mud-forth program finishes executing.
   In cases where they don't, leave a value, we just return a 0.  Note:
   this stuff does not return string or whatnot.  It at most can be
   relied on to return a boolean value.

   interp sets up a player's frames and so on and prepares it for
   execution.
   */

/* The static variable 'err' defined below means to die immediately when
 * set to this value. do_abort_silent() uses this.
 *
 * Otherwise err++ seems popular.
 */
#define ERROR_DIE_NOW -1

void
p_null(PRIM_PROTOTYPE)
{
	return;
}

void
localvar_freeall(struct frame *fr)
{
	struct localvars *ptr;
	struct localvars *nxt;

	ptr = fr->lvars;

	while (ptr) {
		int count = MAX_VAR;
		nxt = ptr->next;
		while (count-- > 0)
			CLEAR(&ptr->lvars[count]);
		ptr->next = NULL;
		ptr->prev = NULL;
		ptr->prog = NOTHING;
		free((void*)ptr);
		ptr = nxt;
	}
	fr->lvars = NULL;
}

void
scopedvar_addlevel(struct frame *fr, struct inst *pc, int count)
{
	struct scopedvar_t *tmp;
	int siz;
	siz = sizeof(struct scopedvar_t) + (sizeof(struct inst) * (count - 1));

	tmp = (struct scopedvar_t *) malloc(siz);
	tmp->count = count;
	tmp->varnames = pc->data.mufproc->varnames;
	tmp->next = fr->svars;
	fr->svars = tmp;
	while (count-- > 0) {
		tmp->vars[count].type = PROG_INTEGER;
		tmp->vars[count].data.number = 0;
	}
}

void
scopedvar_freeall(struct frame *fr)
{
	while (scopedvar_poplevel(fr)) ;
}

int
scopedvar_poplevel(struct frame *fr)
{
	struct scopedvar_t *tmp;

	if (!fr->svars) {
		return 0;
	}

	tmp = fr->svars;
	fr->svars = fr->svars->next;
	while (tmp->count-- > 0) {
		CLEAR(&tmp->vars[tmp->count]);
	}
	free(tmp);
	return 1;
}

struct inst *
scopedvar_get(struct frame *fr, int level, int varnum)
{
	struct scopedvar_t *svinfo = fr->svars;
	while (svinfo && level-->0)
		svinfo = svinfo->next;
	if (!svinfo) {
		return NULL;
	}
	if (varnum < 0 || varnum >= svinfo->count) {
		return NULL;
	}
	return (&svinfo->vars[varnum]);
}

const char*
scopedvar_getname(struct frame *fr, int level, int varnum)
{
	struct scopedvar_t *svinfo = fr->svars;

	while (svinfo && level-->0)
		svinfo = svinfo->next;
	if (!svinfo) {
		return NULL;
	}
	if (varnum < 0 || varnum >= svinfo->count) {
		return NULL;
	}
	if (!svinfo->varnames) {
		return NULL;
	}
	return svinfo->varnames[varnum];
}

void
RCLEAR(struct inst *oper, char *file, int line)
{
	int varcnt, j;

	assert(oper != NULL);
	assert(file != NULL);
	assert(line > 0);

	switch (oper->type) {
	case PROG_CLEARED: {
		time_t lt;
		char buf[40];

		lt = time(NULL);
		strftime(buf, 32, "%c", localtime(&lt));
		fprintf(stderr, "%.32s: ", buf);
		fprintf(stderr, "Attempt to re-CLEAR() instruction from %s:%d "
				"previously CLEAR()ed at %s:%d\n", file, line, (char *) oper->data.addr,
				oper->line);
		assert(0); /* If debugging, we want to figure out just what
					  is going on, and dump core at this point.  This
					  will at least give us some idea of what's going on. */	
		return;
		}
	case PROG_ADD:
		PROGRAM_DEC_INSTANCES(oper->data.addr->progref);
		oper->data.addr->links--;
		break;
	case PROG_STRING:
		if (oper->data.string && --oper->data.string->links == 0)
			free((void *) oper->data.string);
		break;
	case PROG_FUNCTION:
		if (oper->data.mufproc) {
			free((void*) oper->data.mufproc->procname);
			varcnt = oper->data.mufproc->vars;
			if (oper->data.mufproc->varnames) {
				for (j = 0; j < varcnt; j++) {
					free((void*)oper->data.mufproc->varnames[j]);
				}
				free((void*) oper->data.mufproc->varnames);
			}
			free((void*) oper->data.mufproc);
		}
		break;
	case PROG_ARRAY:
		array_free(oper->data.array);
		break;
	case PROG_LOCK:
		if (oper->data.lock != TRUE_BOOLEXP)
			free_boolexp(oper->data.lock);
		break;
	}
	oper->line = line;
	oper->data.addr = (struct prog_addr *) file;
	oper->type = PROG_CLEARED;
}

void push(struct inst *stack, int *top, int type, voidptr res);

int top_pid = 1;
int nargs = 0;

static struct frame *free_frames_list = NULL;

struct forvars *for_pool = NULL;
struct forvars **last_for = &for_pool;
struct tryvars *try_pool = NULL;
struct tryvars **last_try = &try_pool;

void
purge_free_frames(void)
{
	struct frame *ptr, *ptr2;
	int count = FREE_FRAMES_POOL;

	for (ptr = free_frames_list; ptr && --count > 0; ptr = ptr->next) ;
	while (ptr && ptr->next) {
		ptr2 = ptr->next;
		ptr->next = ptr->next->next;
		free(ptr2);
	}
}

void
purge_all_free_frames(void)
{
	struct frame *ptr;

	while (free_frames_list) {
		ptr = free_frames_list;
		free_frames_list = ptr->next;
		free(ptr);
	}
}

void
purge_for_pool(void)
{
	/* This only purges up to the most recently used. */
	/* Purge this a second time to purge all. */
	struct forvars *cur, *next;

	cur = *last_for;
	*last_for = NULL;
	last_for = &for_pool;

	while (cur) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}


void
purge_try_pool(void)
{
	/* This only purges up to the most recently used. */
	/* Purge this a second time to purge all. */
	struct tryvars *cur, *next;

	cur = *last_try;
	*last_try = NULL;
	last_try = &try_pool;

	while (cur) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}



struct frame *
interp(int descr, dbref player, dbref location, dbref program,
	   dbref source, int nosleeps, int whichperms, int forced_pid)
{
	struct frame *fr;
	int i;

	if (!MLevel(program) || !MLevel(OWNER(program)) ||
		((source != NOTHING) && !TrueWizard(OWNER(source)) &&
		 !can_link_to(OWNER(source), TYPE_EXIT, program))) {
		notify_nolisten(player, "Program call: Permission denied.", 1);
		return 0;
	}
	if (free_frames_list) {
		fr = free_frames_list;
		free_frames_list = fr->next;
	} else {
		fr = (struct frame *) malloc(sizeof(struct frame));
	}
	fr->next = NULL;
	fr->pid = forced_pid ? forced_pid : top_pid++;
	fr->descr = descr;
	fr->multitask = nosleeps;
	fr->perms = whichperms;
	fr->already_created = 0;
	fr->been_background = (nosleeps == 2);
	fr->trig = source;
	fr->events = NULL;
	fr->timercount = 0;
	fr->started = time(NULL);
	fr->instcnt = 0;
	fr->skip_declare = 0;
	fr->wantsblanks = 0;
	fr->caller.top = 1;
	fr->caller.st[0] = source;
	fr->caller.st[1] = program;

	fr->system.top = 1;
	fr->system.st[0].progref = 0;
	fr->system.st[0].offset = 0;

	fr->waitees = NULL;
	fr->waiters = NULL;

	fr->fors.top = 0;
	fr->fors.st = NULL;
	fr->trys.top = 0;
	fr->trys.st = NULL;

	fr->errorstr = NULL;
	fr->errorinst = NULL;
	fr->errorprog = NOTHING;
	fr->errorline = 0;

	fr->rndbuf = NULL;
	fr->dlogids = NULL;

	fr->argument.top = 0;
	fr->pc = PROGRAM_START(program);
	fr->writeonly = ((source == -1) || (Typeof(source) == TYPE_ROOM) ||
					 ((Typeof(source) == TYPE_PLAYER) && (!online(source))) ||
					 (FLAGS(player) & READMODE));
	fr->level = 0;
	fr->error.is_flags = 0;

	/* set basic local variables */

	fr->svars = NULL;
	fr->lvars = NULL;
	for (i = 0; i < MAX_VAR; i++) {
		fr->variables[i].type = PROG_INTEGER;
		fr->variables[i].data.number = 0;
	}

	fr->brkpt.force_debugging = 0;
	fr->brkpt.debugging = 0;
	fr->brkpt.bypass = 0;
	fr->brkpt.isread = 0;
	fr->brkpt.showstack = 0;
	fr->brkpt.dosyspop = 0;
	fr->brkpt.lastline = 0;
	fr->brkpt.lastpc = 0;
	fr->brkpt.lastlisted = 0;
	fr->brkpt.lastcmd = NULL;
	fr->brkpt.breaknum = -1;

	fr->brkpt.lastproglisted = NOTHING;
	fr->brkpt.proglines = NULL;

	fr->brkpt.count = 1;
	fr->brkpt.temp[0] = 1;
	fr->brkpt.level[0] = -1;
	fr->brkpt.line[0] = -1;
	fr->brkpt.linecount[0] = -2;
	fr->brkpt.pc[0] = NULL;
	fr->brkpt.pccount[0] = -2;
	fr->brkpt.prog[0] = program;

	fr->proftime.tv_sec = 0;
    fr->proftime.tv_usec = 0;
    fr->totaltime.tv_sec = 0;
    fr->totaltime.tv_usec = 0;

	fr->variables[0].type = PROG_OBJECT;
	fr->variables[0].data.objref = player;
	fr->variables[1].type = PROG_OBJECT;
	fr->variables[1].data.objref = location;
	fr->variables[2].type = PROG_OBJECT;
	fr->variables[2].data.objref = source;
	fr->variables[3].type = PROG_STRING;
	fr->variables[3].data.string = (!*match_cmdname) ? 0 : alloc_prog_string(match_cmdname);

	if (PROGRAM_CODE(program)) {
		PROGRAM_INC_PROF_USES(program);
	}
	PROGRAM_INC_INSTANCES(program);
	push(fr->argument.st, &(fr->argument.top), PROG_STRING, 
		 MIPSCAST alloc_prog_string(match_args));
	return fr;
}

static int err;
int already_created;

struct forvars *
copy_fors(struct forvars *forstack)
{
	struct forvars *in;
	struct forvars *out = NULL;
	struct forvars *nu;
	struct forvars *last = NULL;

	for (in = forstack; in; in = in->next) {
		if (!for_pool) {
			nu = (struct forvars *) malloc(sizeof(struct forvars));
		} else {
			nu = for_pool;
			if (*last_for == for_pool->next) {
				last_for = &for_pool;
			}
			for_pool = nu->next;
		}

		nu->didfirst = in->didfirst;
		copyinst(&in->cur, &nu->cur);
		copyinst(&in->end, &nu->end);
		nu->step = in->step;
		nu->next = NULL;

		if (!out) {
			last = out = nu;
		} else {
			last->next = nu;
			last = nu;
		}
	}
	return out;
}

struct forvars *
push_for(struct forvars *forstack)
{
	struct forvars *nu;

	if (!for_pool) {
		nu = (struct forvars *) malloc(sizeof(struct forvars));
	} else {
		nu = for_pool;
		if (*last_for == for_pool->next) {
			last_for = &for_pool;
		}
		for_pool = nu->next;
	}
	nu->next = forstack;
	return nu;
}

struct forvars *
pop_for(struct forvars *forstack)
{
	struct forvars *newstack;

	if (!forstack) {
		return NULL;
	}
	newstack = forstack->next;
	forstack->next = for_pool;
	for_pool = forstack;
 	if (last_for == &for_pool) {
 		last_for = &(for_pool->next);
 	}
	return newstack;
}


struct tryvars *
copy_trys(struct tryvars *trystack)
{
	struct tryvars *in;
	struct tryvars *out = NULL;
	struct tryvars *nu;
	struct tryvars *last = NULL;

	for (in = trystack; in; in = in->next) {
		if (!try_pool) {
			nu = (struct tryvars*) malloc(sizeof(struct tryvars));
		} else {
			nu = try_pool;
			if (*last_try == try_pool->next) {
				last_try = &try_pool;
			}
			try_pool = nu->next;
		}

		nu->depth      = in->depth;
		nu->call_level = in->call_level;
		nu->for_count  = in->for_count;
		nu->addr       = in->addr;
		nu->next = NULL;

		if (!out) {
			last = out = nu;
		} else {
			last->next = nu;
			last = nu;
		}
	}
	return out;
}

struct tryvars *
push_try(struct tryvars *trystack)
{
	struct tryvars *nu;

	if (!try_pool) {
		nu = (struct tryvars*) malloc(sizeof(struct tryvars));
	} else {
		nu = try_pool;
		if (*last_try == try_pool->next) {
			last_try = &try_pool;
		}
		try_pool = nu->next;
	}
	nu->next = trystack;
	return nu;
}

struct tryvars *
pop_try(struct tryvars *trystack)
{
	struct tryvars *newstack;

	if (!trystack) {
		return NULL;
	}
	newstack = trystack->next;
	trystack->next = try_pool;
	try_pool = trystack;
 	if (last_try == &try_pool) {
 		last_try = &(try_pool->next);
 	}
	return newstack;
}


/* clean up lists from watchpid and sends event */
void
watchpid_process(struct frame *fr)
{
	struct frame *frame;
	struct mufwatchpidlist *cur;
	struct mufwatchpidlist **curptr;
	struct inst temp1;
	temp1.type = PROG_INTEGER;
	temp1.data.number = fr->pid;

	while (fr->waitees) {
		cur = fr->waitees;
		fr->waitees = cur->next;

		frame = timequeue_pid_frame (cur->pid);
		free (cur);
		if (frame) {
			for (curptr = &frame->waiters; *curptr; curptr = &(*curptr)->next) {
				if ((*curptr)->pid == fr->pid) {
					cur = *curptr;
					*curptr = (*curptr)->next;
					free (cur);
					break;
				}
			}
		}
	}

	while (fr->waiters) {
		char buf[64];

		snprintf (buf, sizeof(buf), "PROC.EXIT.%d", fr->pid);

		cur = fr->waiters;
		fr->waiters = cur->next;

		frame = timequeue_pid_frame (cur->pid);
		free (cur);
		if (frame) {
			muf_event_add(frame, buf, &temp1, 0);
			for (curptr = &frame->waitees; *curptr; curptr = &(*curptr)->next) {
				if ((*curptr)->pid == fr->pid) {
					cur = *curptr;
					*curptr = (*curptr)->next;
					free (cur);
					break;
				}
			}
		}
	}
}


/* clean up the stack. */
void
prog_clean(struct frame *fr)
{
	int i;
	struct frame *ptr;

	for (ptr = free_frames_list; ptr; ptr = ptr->next) {
		if (ptr == fr) {
			time_t lt;
			char buf[40];

			lt = time(NULL);
			strftime(buf, 32, "%c", localtime(&lt));
			fprintf(stderr, "%.32s: ", buf);
			fprintf(stderr, "prog_clean(): Tried to free an already freed program frame!\n");
			abort();
		}
	}

	watchpid_process (fr);

	fr->system.top = 0;
	for (i = 0; i < fr->argument.top; i++){
		CLEAR(&fr->argument.st[i]);
	}

	debug("prog_clean: fr->caller.top=%d\n",fr->caller.top);
	for (i = 1; i <= fr->caller.top; i++) {
		debug("Decreasing instances of fr->caller.st[%d](#%d)\n",
						i, fr->caller.st[i]);
		PROGRAM_DEC_INSTANCES(fr->caller.st[i]);
	}

	for (i = 0; i < MAX_VAR; i++)
		CLEAR(&fr->variables[i]);

	localvar_freeall(fr);
	scopedvar_freeall(fr);

	if (fr->fors.st) {
		struct forvars **loop = &(fr->fors.st);

		while (*loop) {
			CLEAR(&((*loop)->cur));
			CLEAR(&((*loop)->end));
			loop = &((*loop)->next);
		}
		*loop = for_pool;
		if (last_for == &for_pool) {
			last_for = loop;
		}
		for_pool = fr->fors.st;
		fr->fors.st = NULL;
		fr->fors.top = 0;
	}

	if (fr->trys.st) {
		struct tryvars **loop = &(fr->trys.st);

		while (*loop) {
			loop = &((*loop)->next);
		}
		*loop = try_pool;
		if (last_try == &try_pool) {
			last_try = loop;
		}
		try_pool = fr->trys.st;
		fr->trys.st = NULL;
		fr->trys.top = 0;
	}

	fr->argument.top = 0;
	fr->pc = 0;
	if (fr->brkpt.lastcmd)
		free(fr->brkpt.lastcmd);
	if (fr->brkpt.proglines) {
		free_prog_text(fr->brkpt.proglines);
		fr->brkpt.proglines = NULL;
	}

	if (fr->rndbuf)
		delete_seed(fr->rndbuf);

	dequeue_timers(fr->pid, NULL);

	muf_event_purge(fr);
	fr->next = free_frames_list;
	free_frames_list = fr;
	err = 0;
}


void
reload(struct frame *fr, int atop, int stop)
{
	fr->argument.top = atop;
	fr->system.top = stop;
}


void
copyinst(struct inst *from, struct inst *to)
{
	int j, varcnt;
	*to = *from;
	switch(from->type) {
	case PROG_FUNCTION:
	    if (from->data.mufproc) {
			to->data.mufproc = (struct muf_proc_data*)malloc(sizeof(struct muf_proc_data));
			to->data.mufproc->procname = strdup(from->data.mufproc->procname);
			to->data.mufproc->vars = varcnt = from->data.mufproc->vars;
			to->data.mufproc->args = from->data.mufproc->args;
			to->data.mufproc->varnames = (const char**)calloc(varcnt, sizeof(const char*));
			for (j = 0; j < varcnt; j++) {
				to->data.mufproc->varnames[j] = strdup(from->data.mufproc->varnames[j]);
			}
		}
		break;
	case PROG_STRING:
	    if (from->data.string) {
			from->data.string->links++;
		}
		break;
	case PROG_ARRAY:
	    if (from->data.array) {
			from->data.array->links++;
		}
		break;
	case PROG_ADD:
		from->data.addr->links++;
		PROGRAM_INC_INSTANCES(from->data.addr->progref);
		break;
	case PROG_LOCK:
	    if (from->data.lock != TRUE_BOOLEXP) {
			to->data.lock = copy_bool(from->data.lock);
		}
		break;
	}
}


void
copyvars(vars * from, vars * to)
{
	int i;

	for (i = 0; i < MAX_VAR; i++) {
		copyinst(&(*from)[i], &(*to)[i]);
	}
}


void
calc_profile_timing(dbref prog, struct frame *fr)
{
	struct timeval tv;
	struct timeval tv2;

	gettimeofday(&tv, NULL);
	if (tv.tv_usec < fr->proftime.tv_usec) {
		tv.tv_usec += 1000000;
		tv.tv_sec -= 1;
	}
	tv.tv_usec -= fr->proftime.tv_usec;
	tv.tv_sec -= fr->proftime.tv_sec;
	tv2 = PROGRAM_PROFTIME(prog);
	tv2.tv_sec += tv.tv_sec;
	tv2.tv_usec += tv.tv_usec;
	if (tv2.tv_usec >= 1000000) {
		tv2.tv_usec -= 1000000;
		tv2.tv_sec += 1;
	}
	PROGRAM_SET_PROFTIME(prog, tv2.tv_sec, tv2.tv_usec);
	fr->totaltime.tv_sec += tv.tv_sec;
	fr->totaltime.tv_usec += tv.tv_usec;
	if (fr->totaltime.tv_usec > 1000000) {
		fr->totaltime.tv_usec -= 1000000;
		fr->totaltime.tv_sec += 1;
	}
}

struct inst *
interp_loop(dbref player, dbref program, struct frame *fr, int rettyp)
{
	return NULL;
}

void
push(struct inst *stack, int *top, int type, voidptr res)
{
	stack[*top].type = type;
	if (type == PROG_FLOAT)
		stack[*top].data.fnumber = *(double *) res;
	else if (type < PROG_STRING)
		stack[*top].data.number = *(int *) res;
	else
		stack[*top].data.string = (struct shared_string *) res;
	(*top)++;
}


dbref
find_mlev(dbref prog, struct frame * fr, int st)
{
	if ((FLAGS(prog) & STICKY) && (FLAGS(prog) & HAVEN)) {
		if ((st > 1) && (TrueWizard(OWNER(prog))))
			return (find_mlev(fr->caller.st[st - 1], fr, st - 1));
	}
	if (MLevel(prog) < MLevel(OWNER(prog))) {
		return (MLevel(prog));
	} else {
		return (MLevel(OWNER(prog)));
	}
}


/*
 * Errors set with this will not be caught.
 *
 * This will always result in program termination the next time
 * interp_loop() checks for this.
 */
void
do_abort_silent(void)
{
	err = ERROR_DIE_NOW;
}
static const char *interp_c_version = "$RCSfile$ $Revision: 1.58 $";
const char *get_interp_c_version(void) { return interp_c_version; }
