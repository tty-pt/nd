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

extern int top_pid;
int process_count = 0;

extern char *time_format_2(long dt);

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
