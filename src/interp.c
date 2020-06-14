/* Muf Interpreter and dispatcher. */

#include "copyright.h"
#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "db.h"
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
	return 0;
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

static const char *interp_c_version = "$RCSfile$ $Revision: 1.58 $";
const char *get_interp_c_version(void) { return interp_c_version; }
