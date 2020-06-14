/*
  Timequeue event code by Foxen
*/

#include "copyright.h"
#include "config.h"
#include "params.h"
#include "match.h"

#include "db.h"
#include "defaults.h"
#include "mpi.h"
#include "props.h"
#include "interface.h"
#include "externs.h"
#include "interp.h"
#define NDEBUG
#include "debug.h"

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>


#define TQ_MUF_TYP 0
#define TQ_MPI_TYP 1

#define TQ_MUF_QUEUE    0x0
#define TQ_MUF_DELAY    0x1
#define TQ_MUF_LISTEN   0x2
#define TQ_MUF_READ     0x3
#define TQ_MUF_TREAD    0x4
#define TQ_MUF_TIMER    0x5

#define TQ_MPI_QUEUE    0x0
#define TQ_MPI_DELAY    0x1

#define TQ_MPI_SUBMASK  0x7
#define TQ_MPI_LISTEN   0x8
#define TQ_MPI_OMESG   0x10
#define TQ_MPI_BLESSED 0x20


/*
 * Events types and data:
 *  What, typ, sub, when, user, where, trig, prog, frame, str1, cmdstr, str3
 *  qmpi   1    0   1     user  loc    trig  --    --     mpi   cmd     arg
 *  dmpi   1    1   when  user  loc    trig  --    --     mpi   cmd     arg
 *  lmpi   1    8   1     spkr  loc    lstnr --    --     mpi   cmd     heard
 *  oqmpi  1   16   1     user  loc    trig  --    --     mpi   cmd     arg
 *  odmpi  1   17   when  user  loc    trig  --    --     mpi   cmd     arg
 *  olmpi  1   24   1     spkr  loc    lstnr --    --     mpi   cmd     heard
 *  qmuf   0    0   0     user  loc    trig  prog  --     stk_s cmd@    --
 *  lmuf   0    1   0     spkr  loc    lstnr prog  --     heard cmd@    --
 *  dmuf   0    2   when  user  loc    trig  prog  frame  mode  --      --
 *  rmuf   0    3   -1    user  loc    trig  prog  frame  mode  --      --
 *  trmuf  0    4   when  user  loc    trig  prog  frame  mode  --      --
 *  tevmuf 0    5   when  user  loc    trig  prog  frame  mode  event   --
 */


typedef struct timenode {
	struct timenode *next;
	int typ;
	int subtyp;
	time_t when;
	int descr;
	dbref called_prog;
	char *called_data;
	char *command;
	char *str3;
	dbref uid;
	dbref loc;
	dbref trig;
	struct frame *fr;
	struct inst *where;
	int eventnum;
} *timequeue;

static timequeue tqhead = NULL;

void prog_clean(struct frame *fr);

extern int top_pid;
int process_count = 0;

static timequeue free_timenode_list = NULL;
static int free_timenode_count = 0;

static timequeue
alloc_timenode(int typ, int subtyp, time_t mytime, int descr, dbref player,
			   dbref loc, dbref trig, dbref program, struct frame *fr,
			   const char *strdata, const char *strcmd, const char *str3, timequeue nextone)
{
	timequeue ptr;

	if (free_timenode_list) {
		ptr = free_timenode_list;
		free_timenode_list = ptr->next;
		free_timenode_count--;
	} else {
		ptr = (timequeue) malloc(sizeof(struct timenode));
	}
	ptr->typ = typ;
	ptr->subtyp = subtyp;
	ptr->when = mytime;
	ptr->uid = player;
	ptr->loc = loc;
	ptr->trig = trig;
	ptr->descr = descr;
	ptr->fr = fr;
	ptr->called_prog = program;
	ptr->called_data = (char *) strdup((char *) strdata);
	ptr->command = alloc_string(strcmd);
	ptr->str3 = alloc_string(str3);
	ptr->eventnum = (fr) ? fr->pid : top_pid++;
	ptr->next = nextone;
	return (ptr);
}

static void
free_timenode(timequeue ptr)
{
	if (ptr->command)
		free(ptr->command);
	if (ptr->called_data)
		free(ptr->called_data);
	if (ptr->str3)
		free(ptr->str3);
	if (ptr->fr) {
		if (ptr->fr->multitask != BACKGROUND)
			PLAYER_SET_BLOCK(ptr->uid, 0);
		prog_clean(ptr->fr);
	}
	if (free_timenode_count < FREE_FRAMES_POOL) {
		ptr->next = free_timenode_list;
		free_timenode_list = ptr;
		free_timenode_count++;
	} else {
		free(ptr);
	}
}

int
control_process(dbref player, int pid)
{
	timequeue tmp, ptr = tqhead;

	tmp = ptr;
	while ((ptr) && (pid != ptr->eventnum)) {
		tmp = ptr;
		ptr = ptr->next;
	}

	/* If the process isn't in the timequeue, that means it's
		waiting for an event, so let the event code handle
		it. */

	if (!ptr) {
		return muf_event_controls(player, pid);
	}

	/* However, if it is in the timequeue, we have to handle it.
		Other than a Wizard, there are three people who can kill it:
		the owner of the program, the owner of the trigger, and the
		person who is currently running it. */

	if (!controls(player, ptr->called_prog) && !controls(player, ptr->trig)
			&& (player != ptr->uid)) {
		return 0;
	}
	return 1;
}


int
add_event(int event_typ, int subtyp, int dtime, int descr, dbref player, dbref loc,
		  dbref trig, dbref program, struct frame *fr,
		  const char *strdata, const char *strcmd, const char *str3)
{
	timequeue ptr = tqhead;
	timequeue lastevent = NULL;
	time_t rtime = time((time_t *) NULL) + (time_t) dtime;
	int mypids = 0;

	for (ptr = tqhead, mypids = 0; ptr; ptr = ptr->next) {
		if (ptr->uid == player)
			mypids++;
		lastevent = ptr;
	}

	if (process_count > MAX_PROCESS_LIMIT ||
	    (mypids > MAX_PLYR_PROCESSES && !Wizard(OWNER(player)))) {
		if (fr) {
			if (fr->multitask != BACKGROUND)
				PLAYER_SET_BLOCK(player, 0);
			prog_clean(fr);
		}
		notify_nolisten(player, "Event killed.  Timequeue table full.", 1);
		return 0;
	}
	process_count++;

	if (!tqhead) {
		tqhead = alloc_timenode(event_typ, subtyp, rtime, descr, player, loc, trig,
								program, fr, strdata, strcmd, str3, NULL);
		return (tqhead->eventnum);
	}
	if (rtime < tqhead->when) {
		tqhead = alloc_timenode(event_typ, subtyp, rtime, descr, player, loc, trig,
								program, fr, strdata, strcmd, str3, tqhead);
		return (tqhead->eventnum);
	}

	ptr = tqhead;
	while (ptr->next && rtime >= ptr->next->when)
		ptr = ptr->next;

	ptr->next = alloc_timenode(event_typ, subtyp, rtime, descr, player, loc, trig,
							   program, fr, strdata, strcmd, str3, ptr->next);
	return (ptr->next->eventnum);
}


int
add_mpi_event(int delay, int descr, dbref player, dbref loc, dbref trig,
			  const char *mpi, const char *cmdstr, const char *argstr,
			  int listen_p, int omesg_p, int blessed_p)
{
	int subtyp = TQ_MPI_QUEUE;

	if (delay >= 1) {
		subtyp = TQ_MPI_DELAY;
	}
	if (blessed_p) {
		subtyp |= TQ_MPI_BLESSED;
	}
	if (listen_p) {
		subtyp |= TQ_MPI_LISTEN;
	}
	if (omesg_p) {
		subtyp |= TQ_MPI_OMESG;
	}
	return add_event(TQ_MPI_TYP, subtyp, delay, descr, player, loc, trig,
					 NOTHING, NULL, mpi, cmdstr, argstr);
}

void
next_timequeue_event(void)
{
	timequeue lastevent, event;
	int maxruns = 0;
	int forced_pid = 0;
	time_t rtime;

	time(&rtime);

	lastevent = tqhead;
	while ((lastevent) && (rtime >= lastevent->when) && (maxruns < 10)) {
		lastevent = lastevent->next;
		maxruns++;
	}

	while (tqhead && (tqhead != lastevent) && (maxruns--)) {
		event = tqhead;
		tqhead = tqhead->next;
		process_count--;
		forced_pid = event->eventnum;
		event->eventnum = 0;
		if (event->typ == TQ_MPI_TYP) {
			char cbuf[BUFFER_LEN];
			int ival;

			strlcpy(match_args, event->str3 ? event->str3 : "", sizeof(match_args));
			strlcpy(match_cmdname, event->command ? event->command : "", sizeof(match_cmdname));
			ival = (event->subtyp & TQ_MPI_OMESG) ? MPI_ISPUBLIC : MPI_ISPRIVATE;
			if (event->subtyp & TQ_MPI_BLESSED) {
				ival |= MPI_ISBLESSED;
			}
			if (event->subtyp & TQ_MPI_LISTEN) {
				ival |= MPI_ISLISTENER;
				do_parse_mesg(event->descr, event->uid, event->trig, event->called_data,
							  "(MPIlisten)", cbuf, sizeof(cbuf), ival);
			} else if ((event->subtyp & TQ_MPI_SUBMASK) == TQ_MPI_DELAY) {
				do_parse_mesg(event->descr, event->uid, event->trig, event->called_data,
							  "(MPIdelay)", cbuf, sizeof(cbuf), ival);
			} else {
				do_parse_mesg(event->descr, event->uid, event->trig, event->called_data,
							  "(MPIqueue)", cbuf, sizeof(cbuf), ival);
			}
			if (*cbuf) {
				if (!(event->subtyp & TQ_MPI_OMESG)) {
					notify_filtered(event->uid, event->uid, cbuf, 1);
				} else {
					char bbuf[BUFFER_LEN];
					dbref plyr;

					snprintf(bbuf, sizeof(bbuf), ">> %.4000s %.*s",
							NAME(event->uid),
							(int)(4000 - strlen(NAME(event->uid))),
							pronoun_substitute(event->descr, event->uid, cbuf));
					plyr = DBFETCH(event->loc)->contents;
					for (; plyr != NOTHING; plyr = DBFETCH(plyr)->next) {
						if (Typeof(plyr) == TYPE_PLAYER && plyr != event->uid)
							notify_filtered(event->uid, plyr, bbuf, 0);
					}
				}
			}
		}
		event->fr = NULL;
		free_timenode(event);
	}
}


int
in_timequeue(int pid)
{
	timequeue ptr = tqhead;

	if (!pid)
		return 0;
	if (muf_event_pid_frame(pid))
		return 1;
	if (!tqhead)
		return 0;
	while ((ptr) && (ptr->eventnum != pid))
		ptr = ptr->next;
	if (ptr)
		return 1;
	return 0;
}


long
next_event_time(void)
{
	time_t rtime = time((time_t *) NULL);

	if (tqhead) {
		if (tqhead->when == -1) {
			return (-1L);
		} else if (rtime >= tqhead->when) {
			return (0L);
		} else {
			return ((long) (tqhead->when - rtime));
		}
	}
	return (-1L);
}

extern char *time_format_2(long dt);

/*
 * killmode values:
 *     0: kill all matching processes, MUF or MPI
 *     1: kill all matching MUF processes
 *     2: kill all matching foreground MUF processes
 */
int
dequeue_prog_real(dbref program, int killmode, const char *file, const int line)
{
	int count = 0;
	timequeue tmp, ptr;

	debug("dequeue_prog: tqhead = %p\n", (void *) tqhead);
	while (tqhead) {
		debug("tqhead->uid = #%d\n", tqhead->uid);
		if (tqhead->called_prog != program  && tqhead->uid != program) {
			break;
		}
		if (killmode == 2) {
			if (tqhead->fr && tqhead->fr->multitask == BACKGROUND) {
				break;
			}
		} else if (killmode == 1) {
			if (!tqhead->fr) {
				debug("dequeue_prog: killmode 1, no frame\n");
				break;
			}
		}
		ptr = tqhead;
		tqhead = tqhead->next;
		free_timenode(ptr);
		process_count--;
		count++;
	}

	if (tqhead) {
		for (tmp = tqhead, ptr = tqhead->next; ptr; tmp = ptr, ptr = ptr->next) {
			if (ptr->called_prog != program && ptr->uid != program)
				continue;
			if (killmode == 2) {
				if (ptr->fr && ptr->fr->multitask == BACKGROUND) {
					continue;
				}
			} else if (killmode == 1) {
				if (!ptr->fr) {
					debug("dequeue_prog(2): killmode 1, no frame.\n");
					continue;
				}
			}
			tmp->next = ptr->next;
			free_timenode(ptr);
			process_count--;
			count++;
			ptr = tmp;
		}
	}
	debug("dequeue_prog(3): about to muf_event_dequeue(#%d, %d)\n",program, killmode);
	if (count < (count += muf_event_dequeue(program, killmode)))
			prog_clean(tqhead->fr);
	/* and just to make sure we got them all... otherwise, we need
	 * to rethink what we're doing here. */
	/* assert(PROGRAM_INSTANCES(program) == 0);*/
	return (count);
}


int
dequeue_process(int pid)
{
	timequeue tmp, ptr;
	int deqflag = 0;

	if (!pid)
		return 0;

	tmp = ptr = tqhead;
	while (ptr) {
		if (pid == ptr->eventnum) {
			if (tmp == ptr) {
				tqhead = tmp = tmp->next;
				free_timenode(ptr);
				ptr = tmp;
			} else {
				tmp->next = ptr->next;
				free_timenode(ptr);
				ptr = tmp->next;
			}
			process_count--;
			deqflag = 1;
		} else {
			tmp = ptr;
			ptr = ptr->next;
		}
	}

	if (!deqflag) {
		return 0;
	}

	return 1;
}

static int propq_level = 0;
void
propqueue(int descr, dbref player, dbref where, dbref trigger, dbref what, dbref xclude,
		  const char *propname, const char *toparg, int mlev, int mt)
{
	const char *tmpchar;
	const char *pname;
	dbref the_prog;
	char buf[BUFFER_LEN];
	char exbuf[BUFFER_LEN];

	the_prog = NOTHING;
	tmpchar = NULL;

	/* queue up program referred to by the given property */
	if (((the_prog = get_property_dbref(what, propname)) != NOTHING) ||
		(tmpchar = get_property_class(what, propname))) {

		if ((tmpchar && *tmpchar) || the_prog != NOTHING) {
			if (tmpchar) {
				if (*tmpchar == '&') {
					the_prog = AMBIGUOUS;
				} else if (*tmpchar == NUMBER_TOKEN && number(tmpchar + 1)) {
					the_prog = (dbref) atoi(++tmpchar);
				} else if (*tmpchar == REGISTERED_TOKEN) {
					the_prog = find_registered_obj(what, tmpchar);
				} else if (number(tmpchar)) {
					the_prog = (dbref) atoi(tmpchar);
				} else {
					the_prog = NOTHING;
				}
			} else {
				if (the_prog == AMBIGUOUS)
					the_prog = NOTHING;
			}
			if (the_prog != AMBIGUOUS) {
				if (the_prog < 0 || the_prog >= db_top) {
					the_prog = NOTHING;
				} else if (Typeof(the_prog) != TYPE_PROGRAM) {
					the_prog = NOTHING;
				} else if ((OWNER(the_prog) != OWNER(player)) && !(FLAGS(the_prog) & LINK_OK)) {
					the_prog = NOTHING;
				} else if (MLevel(the_prog) < mlev) {
					the_prog = NOTHING;
				} else if (MLevel(OWNER(the_prog)) < mlev) {
					the_prog = NOTHING;
				} else if (the_prog == xclude) {
					the_prog = NOTHING;
				}
			}
			if (propq_level < 8) {
				propq_level++;
				if (the_prog == AMBIGUOUS) {
					char cbuf[BUFFER_LEN];
					int ival;

					strlcpy(match_args, "", sizeof(match_args));
					strlcpy(match_cmdname, toparg, sizeof(match_cmdname));
					ival = (mt == 0) ? MPI_ISPUBLIC : MPI_ISPRIVATE;
					if (Prop_Blessed(what, propname))
						ival |= MPI_ISBLESSED;
					do_parse_mesg(descr, player, what, tmpchar + 1, "(MPIqueue)", cbuf, sizeof(cbuf), ival);
					if (*cbuf) {
						if (mt) {
							notify_filtered(player, player, cbuf, 1);
						} else {
							char bbuf[BUFFER_LEN];
							dbref plyr;

							snprintf(bbuf, sizeof(bbuf), ">> %.4000s",
									pronoun_substitute(descr, player, cbuf));
							plyr = DBFETCH(where)->contents;
							while (plyr != NOTHING) {
								if (Typeof(plyr) == TYPE_PLAYER && plyr != player)
									notify_filtered(player, plyr, bbuf, 0);
								plyr = DBFETCH(plyr)->next;
							}
						}
					}
				} else if (the_prog != NOTHING) {
					struct frame *tmpfr;

					strlcpy(match_args, toparg ? toparg : "", sizeof(match_args));
					strlcpy(match_cmdname, "Queued event.", sizeof(match_cmdname));
					tmpfr = interp(descr, player, where, the_prog, trigger,
								   BACKGROUND, STD_HARDUID, 0);
					if (tmpfr) {
						interp_loop(player, the_prog, tmpfr, 0);
					}
				}
				propq_level--;
			} else {
				notify_nolisten(player, "Propqueue stopped to prevent infinite loop.", 1);
			}
		}
	}
	strlcpy(buf, propname, sizeof(buf));
	if (is_propdir(what, buf)) {
		strlcat(buf, "/", sizeof(buf));
		while ((pname = next_prop_name(what, exbuf, sizeof(exbuf), buf))) {
			strlcpy(buf, pname, sizeof(buf));
			propqueue(descr, player, where, trigger, what, xclude, buf, toparg, mlev, mt);
		}
	}
}


void
envpropqueue(int descr, dbref player, dbref where, dbref trigger, dbref what, dbref xclude,
			 const char *propname, const char *toparg, int mlev, int mt)
{
	while (what != NOTHING) {
		propqueue(descr, player, where, trigger, what, xclude, propname, toparg, mlev, mt);
		what = getparent(what);
	}
}


void
listenqueue(int descr, dbref player, dbref where, dbref trigger, dbref what, dbref xclude,
			const char *propname, const char *toparg, int mlev, int mt, int mpi_p)
{
	return;
}
static const char *timequeue_c_version = "$RCSfile$ $Revision: 1.45 $";
const char *get_timequeue_c_version(void) { return timequeue_c_version; }
