
/* $Header$ */

#include "copyright.h"
#include "config.h"
#include "params.h"

/* commands which look at things */

#include <ctype.h>
#include <sys/time.h>
#include "db.h"
#include "tune.h"
#include "mpi.h"
#include "props.h"
#include "interface.h"
#include "match.h"
#include "dbsearch.h"
#include "externs.h"



#ifdef COMPRESS
extern const char *uncompress(const char *);

#endif							/* COMPRESS */

#define EXEC_SIGNAL '@'			/* Symbol which tells us what we're looking at
								   * is an execution order and not a message.    */

/* prints owner of something */
static void
print_owner(dbref player, dbref thing)
{
	char buf[BUFFER_LEN];

	switch (Typeof(thing)) {
	case TYPE_PLAYER:
		sprintf(buf, "%s is a player.", NAME(thing));
		break;
	case TYPE_ROOM:
	case TYPE_THING:
	case TYPE_EXIT:
	case TYPE_PROGRAM:
		sprintf(buf, "Owner: %s", NAME(OWNER(thing)));
		break;
	case TYPE_GARBAGE:
		sprintf(buf, "%s is garbage.", NAME(thing));
		break;
	}
	notify(player, buf);
}

void
exec_or_notify_prop(int descr, dbref player, dbref thing,
					const char *propname, const char *whatcalled)
{
	const char *message = get_property_class(thing, propname);
	int mpiflags = Prop_Blessed(thing, propname)? MPI_ISBLESSED : 0;

	if (message)
		exec_or_notify(descr, player, thing, message, whatcalled, mpiflags);
}

void
exec_or_notify(int descr, dbref player, dbref thing,
			   const char *message, const char *whatcalled,
			   int mpiflags)
{
	char *p;
	char *p2;
	char *p3;
	char buf[BUFFER_LEN];
	char tmpcmd[BUFFER_LEN];
	char tmparg[BUFFER_LEN];

#ifdef COMPRESS
	p = (char *) uncompress((char *) message);
#else							/* !COMPRESS */
	p = (char *) message;
#endif							/* COMPRESS */

	if (*p == EXEC_SIGNAL) {
		int i;

		if (*(++p) == REGISTERED_TOKEN) {
			strcpy(buf, p);
			for (p2 = buf; *p && !isspace(*p); p++) ;
			if (*p)
				p++;

			for (p3 = buf; *p3 && !isspace(*p3); p3++) ;
			if (*p3)
				*p3 = '\0';

			if (*p2) {
				i = (dbref) find_registered_obj(thing, p2);
			} else {
				i = 0;
			}
		} else {
			i = atoi(p);
			for (; *p && !isspace(*p); p++) ;
			if (*p)
				p++;
		}
		if (i < 0 || i >= db_top || (Typeof(i) != TYPE_PROGRAM)) {
			if (*p) {
				notify(player, p);
			} else {
				notify(player, "You see nothing special.");
			}
		} else {
			struct frame *tmpfr;

			strcpy(tmparg, match_args);
			strcpy(tmpcmd, match_cmdname);
			p = do_parse_mesg(descr, player, thing, p, whatcalled, buf, MPI_ISPRIVATE | mpiflags);
			strcpy(match_args, p);
			strcpy(match_cmdname, whatcalled);
			tmpfr = interp(descr, player, DBFETCH(player)->location, i, thing,
						   PREEMPT, STD_HARDUID);
			if (tmpfr) {
				interp_loop(player, i, tmpfr, 0);
			}
			strcpy(match_args, tmparg);
			strcpy(match_cmdname, tmpcmd);
		}
	} else {
		p = do_parse_mesg(descr, player, thing, p, whatcalled, buf, MPI_ISPRIVATE | mpiflags);
		notify(player, p);
	}
}

static void
look_contents(dbref player, dbref loc, const char *contents_name)
{
	dbref thing;
	dbref can_see_loc;

	/* check to see if he can see the location */
	can_see_loc = (!Dark(loc) || controls(player, loc));

	/* check to see if there is anything there */
	DOLIST(thing, DBFETCH(loc)->contents) {
		if (can_see(player, thing, can_see_loc)) {
			/* something exists!  show him everything */
			notify(player, contents_name);
			DOLIST(thing, DBFETCH(loc)->contents) {
				if (can_see(player, thing, can_see_loc)) {
					notify(player, unparse_object(player, thing));
				}
			}
			break;				/* we're done */
		}
	}
}

static void
look_simple(int descr, dbref player, dbref thing)
{
	if (GETDESC(thing)) {
		exec_or_notify(descr, player, thing, GETDESC(thing), "(@Desc)",
				Prop_Blessed(thing, MESGPROP_DESC)? MPI_ISBLESSED : 0);
	} else {
		notify(player, "You see nothing special.");
	}
}

void
look_room(int descr, dbref player, dbref loc, int verbose)
{
	char obj_num[20];

	/* tell him the name, and the number if he can link to it */
	notify(player, unparse_object(player, loc));

	/* tell him the description */
	if (Typeof(loc) == TYPE_ROOM) {
		if (GETDESC(loc)) {
			exec_or_notify(descr, player, loc, GETDESC(loc), "(@Desc)",
				Prop_Blessed(loc, MESGPROP_DESC)? MPI_ISBLESSED : 0);
		}
		/* tell him the appropriate messages if he has the key */
		can_doit(descr, player, loc, 0);
	} else {
		if (GETIDESC(loc)) {
			exec_or_notify(descr, player, loc, GETIDESC(loc), "(@Idesc)",
				Prop_Blessed(loc, MESGPROP_IDESC)? MPI_ISBLESSED : 0);
		}
	}
	ts_useobject(loc);

	/* tell him the contents */
	look_contents(player, loc, "Contents:");
	if (tp_look_propqueues) {
		sprintf(obj_num, "#%d", loc);
		envpropqueue(descr, player, loc, player, loc, NOTHING, "_lookq", obj_num, 1, 1);
	}
}

void
do_look_around(int descr, dbref player)
{
	dbref loc;

	if ((loc = getloc(player)) == NOTHING)
		return;
	look_room(descr, player, loc, 1);
}

void
do_look_at(int descr, dbref player, const char *name, const char *detail)
{
	dbref thing;
	struct match_data md;
	/* int res; */
	char buf[BUFFER_LEN];
	char obj_num[20];

	if (*name == '\0' || !string_compare(name, "here")) {
		if ((thing = getloc(player)) != NOTHING) {
			look_room(descr, player, thing, 1);
		}
	} else {
		/* look at a thing here */
		init_match(descr, player, name, NOTYPE, &md);
		match_all_exits(&md);
		match_neighbor(&md);
		match_possession(&md);
		/* match_registered(&md); */
		if (Wizard(OWNER(player))) {
			match_absolute(&md);
			match_player(&md);
		}
		match_here(&md);
		match_me(&md);

		thing = match_result(&md);
		if (thing != NOTHING && thing != AMBIGUOUS && !*detail) {
			switch (Typeof(thing)) {
			case TYPE_ROOM:
				if (getloc(player) != thing && !can_link_to(player, TYPE_ROOM, thing)) {
					notify(player, "Permission denied.");
				} else {
					look_room(descr, player, thing, 1);
				}
				break;
			case TYPE_PLAYER:
				if (getloc(player) != getloc(thing)
					&& !controls(player, thing)) {
					notify(player, "Permission denied.");
				} else {
					look_simple(descr, player, thing);
					look_contents(player, thing, "Carrying:");
					if (tp_look_propqueues) {
						sprintf(obj_num, "#%d", thing);
						envpropqueue(descr, player, thing, player, thing,
									 NOTHING, "_lookq", obj_num, 1, 1);
					}
				}
				break;
			case TYPE_THING:
				if (getloc(player) != getloc(thing)
					&& getloc(thing) != player && !controls(player, thing)) {
					notify(player, "Permission denied.");
				} else {
					look_simple(descr, player, thing);
					if (!(FLAGS(thing) & HAVEN)) {
						look_contents(player, thing, "Contains:");
						ts_useobject(thing);
					}
					if (tp_look_propqueues) {
						sprintf(obj_num, "#%d", thing);
						envpropqueue(descr, player, thing, player, thing,
									 NOTHING, "_lookq", obj_num, 1, 1);
					}
				}
				break;
			default:
				look_simple(descr, player, thing);
				if (Typeof(thing) != TYPE_PROGRAM)
					ts_useobject(thing);
				if (tp_look_propqueues) {
					sprintf(obj_num, "#%d", thing);
					envpropqueue(descr, player, thing, player, thing,
								 NOTHING, "_lookq", obj_num, 1, 1);
				}
				break;
			}
		} else if (thing == NOTHING || (*detail && thing != AMBIGUOUS)) {
			int ambig_flag = 0;
			char propname[BUFFER_LEN];
			PropPtr propadr, pptr, lastmatch = NULL;

			if (thing == NOTHING) {
				thing = getloc(player);
				sprintf(buf, "%s", name);
			} else {
				sprintf(buf, "%s", detail);
			}

#ifdef DISKBASE
			fetchprops(thing, "/_details/");
#endif

			lastmatch = NULL;
			propadr = first_prop(thing, "_details/", &pptr, propname);
			while (propadr) {
				if (exit_prefix(propname, buf)) {
					if (lastmatch) {
						lastmatch = NULL;
						ambig_flag = 1;
						break;
					} else {
						lastmatch = propadr;
					}
				}
				propadr = next_prop(pptr, propadr, propname);
			}
			if (lastmatch && PropType(lastmatch) == PROP_STRTYP) {
#ifdef DISKBASE
				propfetch(thing, lastmatch);	/* DISKBASE PROPVALS */
#endif
				exec_or_notify(descr, player, thing, PropDataStr(lastmatch), "(@detail)",
					(PropFlags(lastmatch) & PROP_BLESSED)? MPI_ISBLESSED : 0);
			} else if (ambig_flag) {
				notify(player, AMBIGUOUS_MESSAGE);
			} else if (*detail) {
				notify(player, "You see nothing special.");
			} else {
				notify(player, NOMATCH_MESSAGE);
			}
		} else {
			notify(player, AMBIGUOUS_MESSAGE);
		}
	}
}

#ifdef VERBOSE_EXAMINE
static const char *
flag_description(dbref thing)
{
	static char buf[BUFFER_LEN];

	strcpy(buf, "Type: ");
	switch (Typeof(thing)) {
	case TYPE_ROOM:
		strcat(buf, "ROOM");
		break;
	case TYPE_EXIT:
		strcat(buf, "EXIT/ACTION");
		break;
	case TYPE_THING:
		strcat(buf, "THING");
		break;
	case TYPE_PLAYER:
		strcat(buf, "PLAYER");
		break;
	case TYPE_PROGRAM:
		strcat(buf, "PROGRAM");
		break;
	case TYPE_GARBAGE:
		strcat(buf, "GARBAGE");
		break;
	default:
		strcat(buf, "***UNKNOWN TYPE***");
		break;
	}

	if (FLAGS(thing) & ~TYPE_MASK) {
		/* print flags */
		strcat(buf, "  Flags:");
		if (FLAGS(thing) & WIZARD)
			strcat(buf, " WIZARD");
		if (FLAGS(thing) & QUELL)
			strcat(buf, " QUELL");
		if (FLAGS(thing) & STICKY)
			strcat(buf, (Typeof(thing) == TYPE_PROGRAM) ? " SETUID" :
				   (Typeof(thing) == TYPE_PLAYER) ? " SILENT" : " STICKY");
		if (FLAGS(thing) & DARK)
			strcat(buf, (Typeof(thing) != TYPE_PROGRAM) ? " DARK" : " DEBUGGING");
		if (FLAGS(thing) & LINK_OK)
			strcat(buf, " LINK_OK");

		if (FLAGS(thing) & KILL_OK)
			strcat(buf, " KILL_OK");

		if (MLevRaw(thing)) {
			strcat(buf, " MUCKER");
			switch (MLevRaw(thing)) {
			case 1:
				strcat(buf, "1");
				break;
			case 2:
				strcat(buf, "2");
				break;
			case 3:
				strcat(buf, "3");
				break;
			}
		}
		if (FLAGS(thing) & BUILDER)
			strcat(buf, (Typeof(thing) == TYPE_PROGRAM) ? " BOUND" : " BUILDER");
		if (FLAGS(thing) & CHOWN_OK)
			strcat(buf, (Typeof(thing) == TYPE_PLAYER) ? " COLOR" : " CHOWN_OK");
		if (FLAGS(thing) & JUMP_OK)
			strcat(buf, " JUMP_OK");
		if (FLAGS(thing) & VEHICLE)
/*			strcat(buf, " VEHICLE"); */
			strcat(buf, (Typeof(thing) == TYPE_PROGRAM) ? " VIEWABLE" : " VEHICLE");

		if (FLAGS(thing) & XFORCIBLE) {
			if (Typeof(thing) == TYPE_EXIT) {
				strcat(buf, " XPRESS");
			} else {
				strcat(buf, " XFORCIBLE");
			}
		}
		if (FLAGS(thing) & ZOMBIE)
			strcat(buf, " ZOMBIE");
		if (FLAGS(thing) & HAVEN)
			strcat(buf,
				   (Typeof(thing) !=
					TYPE_PROGRAM) ? ((Typeof(thing) ==
									  TYPE_THING) ? " HIDE" : " HAVEN") : " HARDUID");
		if (FLAGS(thing) & ABODE)
			strcat(buf,
				   (Typeof(thing) != TYPE_PROGRAM) ? (Typeof(thing) !=
													  TYPE_EXIT ? " ABODE" : " ABATE") :
				   " AUTOSTART");
	}
	return buf;
}

#endif							/* VERBOSE_EXAMINE */

int
listprops_wildcard(dbref player, dbref thing, const char *dir, const char *wild)
{
	char propname[BUFFER_LEN];
	char wld[BUFFER_LEN];
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *ptr, *wldcrd = wld;
	PropPtr propadr, pptr;
	int i, cnt = 0;
	int recurse = 0;

	strcpy(wld, wild);
	i = strlen(wld);
	if (i && wld[i - 1] == PROPDIR_DELIMITER)
		strcat(wld, "*");
	for (wldcrd = wld; *wldcrd == PROPDIR_DELIMITER; wldcrd++) ;
	if (!strcmp(wldcrd, "**"))
		recurse = 1;

	for (ptr = wldcrd; *ptr && *ptr != PROPDIR_DELIMITER; ptr++) ;
	if (*ptr)
		*ptr++ = '\0';

	propadr = first_prop(thing, (char *) dir, &pptr, propname);
	while (propadr) {
		if (equalstr(wldcrd, propname)) {
			sprintf(buf, "%s%c%s", dir, PROPDIR_DELIMITER, propname);
			if ((!Prop_Hidden(buf) && !(PropFlags(propadr) & PROP_SYSPERMS))
				|| Wizard(OWNER(player))) {
				if (!*ptr || recurse) {
					cnt++;
					displayprop(player, thing, buf, buf2);
					notify(player, buf2);
				}
				if (recurse)
					ptr = "**";
				cnt += listprops_wildcard(player, thing, buf, ptr);
			}
		}
		propadr = next_prop(pptr, propadr, propname);
	}
	return cnt;
}


long
size_object(dbref i, int load)
{
	long byts;
	byts = sizeof(struct object);

	if (NAME(i)) {
		byts += strlen(NAME(i)) + 1;
	}
	byts += size_properties(i, load);

	if (Typeof(i) == TYPE_EXIT && DBFETCH(i)->sp.exit.dest) {
		byts += sizeof(dbref) * DBFETCH(i)->sp.exit.ndest;
	} else if (Typeof(i) == TYPE_PLAYER && PLAYER_PASSWORD(i)) {
		byts += strlen(PLAYER_PASSWORD(i)) + 1;
	} else if (Typeof(i) == TYPE_PROGRAM) {
		byts += size_prog(i);
	}
	return byts;
}


void
do_examine(int descr, dbref player, const char *name, const char *dir)
{
	dbref thing;
	char buf[BUFFER_LEN];
	dbref content;
	dbref exit;
	int i, cnt;
	struct match_data md;
	struct tm *time_tm;			/* used for timestamps */

	if (*name == '\0') {
		if ((thing = getloc(player)) == NOTHING)
			return;
	} else {
		/* look it up */
		init_match(descr, player, name, NOTYPE, &md);

		match_all_exits(&md);
		match_neighbor(&md);
		match_possession(&md);
		match_absolute(&md);
		match_registered(&md);

		/* only Wizards can examine other players */
		if (Wizard(OWNER(player)))
			match_player(&md);

		match_here(&md);
		match_me(&md);

		/* get result */
		if ((thing = noisy_match_result(&md)) == NOTHING)
			return;
	}

	if (!can_link(player, thing)) {
		print_owner(player, thing);
		return;
	}
	if (*dir) {
		/* show him the properties */
		cnt = listprops_wildcard(player, thing, "", dir);
		sprintf(buf, "%d propert%s listed.", cnt, (cnt == 1 ? "y" : "ies"));
		notify(player, buf);
		return;
	}
	switch (Typeof(thing)) {
	case TYPE_ROOM:
		sprintf(buf, "%.*s  Owner: %s  Parent: ",
				(BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				NAME(OWNER(thing)));
		strcat(buf, unparse_object(player, DBFETCH(thing)->location));
		break;
	case TYPE_THING:
		sprintf(buf, "%.*s  Owner: %s  Value: %d",
				(BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				NAME(OWNER(thing)), THING_VALUE(thing));
		break;
	case TYPE_PLAYER:
		sprintf(buf, "%.*s  %s: %d  ", 
				(BUFFER_LEN - strlen(tp_cpennies) - 35),
				unparse_object(player, thing),
				tp_cpennies, PLAYER_PENNIES(thing));
		break;
	case TYPE_EXIT:
	case TYPE_PROGRAM:
		sprintf(buf, "%.*s  Owner: %s",
				(BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				NAME(OWNER(thing)));
		break;
	case TYPE_GARBAGE:
		strcpy(buf, unparse_object(player, thing));
		break;
	}
	notify(player, buf);

#ifdef VERBOSE_EXAMINE
	notify(player, flag_description(thing));
#endif							/* VERBOSE_EXAMINE */

	if (GETDESC(thing))
		notify(player, GETDESC(thing));
	sprintf(buf, "Key: %s", unparse_boolexp(player, GETLOCK(thing), 1));
	notify(player, buf);

	sprintf(buf, "Chown_OK Key: %s",
			unparse_boolexp(player, get_property_lock(thing, "_/chlk"), 1));
	notify(player, buf);

	sprintf(buf, "Container Key: %s",
			unparse_boolexp(player, get_property_lock(thing, "_/clk"), 1));
	notify(player, buf);

	sprintf(buf, "Force Key: %s",
			unparse_boolexp(player, get_property_lock(thing, "@/flk"), 1));
	notify(player, buf);

	if (GETSUCC(thing)) {
		sprintf(buf, "Success: %s", GETSUCC(thing));
		notify(player, buf);
	}
	if (GETFAIL(thing)) {
		sprintf(buf, "Fail: %s", GETFAIL(thing));
		notify(player, buf);
	}
	if (GETDROP(thing)) {
		sprintf(buf, "Drop: %s", GETDROP(thing));
		notify(player, buf);
	}
	if (GETOSUCC(thing)) {
		sprintf(buf, "Osuccess: %s", GETOSUCC(thing));
		notify(player, buf);
	}
	if (GETOFAIL(thing)) {
		sprintf(buf, "Ofail: %s", GETOFAIL(thing));
		notify(player, buf);
	}
	if (GETODROP(thing)) {
		sprintf(buf, "Odrop: %s", GETODROP(thing));
		notify(player, buf);
	}

	if (tp_who_doing && GETDOING(thing)) {
		sprintf(buf, "Doing: %s", GETDOING(thing));
		notify(player, buf);
	}
	if (tp_who_doing && GETOECHO(thing)) {
		sprintf(buf, "Oecho: %s", GETOECHO(thing));
		notify(player, buf);
	}
	if (tp_who_doing && GETPECHO(thing)) {
		sprintf(buf, "Pecho: %s", GETPECHO(thing));
		notify(player, buf);
	}

	/* Timestamps */
	/* ex: time_tm = localtime((time_t *)(&(DBFETCH(thing)->ts.created))); */

	time_tm = localtime((&(DBFETCH(thing)->ts.created)));
	(void) format_time(buf, BUFFER_LEN, (char *) "Created:  %a %b %e %T %Z %Y", time_tm);
	notify(player, buf);
	time_tm = localtime((&(DBFETCH(thing)->ts.modified)));
	(void) format_time(buf, BUFFER_LEN, (char *) "Modified: %a %b %e %T %Z %Y", time_tm);
	notify(player, buf);
	time_tm = localtime((&(DBFETCH(thing)->ts.lastused)));
	(void) format_time(buf, BUFFER_LEN, (char *) "Lastused: %a %b %e %T %Z %Y", time_tm);
	notify(player, buf);
	if (Typeof(thing) == TYPE_PROGRAM) {
		sprintf(buf, "Usecount: %d     Instances: %d",
				DBFETCH(thing)->ts.usecount, PROGRAM_INSTANCES(thing));
	} else {
		sprintf(buf, "Usecount: %d", DBFETCH(thing)->ts.usecount);
	}
	notify(player, buf);

	notify(player, "[ Use 'examine <object>=/' to list root properties. ]");

	sprintf(buf, "Memory used: %ld bytes", size_object(thing, 1));
	notify(player, buf);

	/* show him the contents */
	if (DBFETCH(thing)->contents != NOTHING) {
		if (Typeof(thing) == TYPE_PLAYER)
			notify(player, "Carrying:");
		else
			notify(player, "Contents:");
		DOLIST(content, DBFETCH(thing)->contents) {
			notify(player, unparse_object(player, content));
		}
	}
	switch (Typeof(thing)) {
	case TYPE_ROOM:
		/* tell him about exits */
		if (DBFETCH(thing)->exits != NOTHING) {
			notify(player, "Exits:");
			DOLIST(exit, DBFETCH(thing)->exits) {
				notify(player, unparse_object(player, exit));
			}
		} else {
			notify(player, "No exits.");
		}

		/* print dropto if present */
		if (DBFETCH(thing)->sp.room.dropto != NOTHING) {
			sprintf(buf, "Dropped objects go to: %s",
					unparse_object(player, DBFETCH(thing)->sp.room.dropto));
			notify(player, buf);
		}
		break;
	case TYPE_THING:
		/* print home */
		sprintf(buf, "Home: %s", unparse_object(player, THING_HOME(thing)));	/* home */
		notify(player, buf);
		/* print location if player can link to it */
		if (DBFETCH(thing)->location != NOTHING && (controls(player, DBFETCH(thing)->location)
													|| can_link_to(player, NOTYPE,
																   DBFETCH(thing)->location))) {
			sprintf(buf, "Location: %s", unparse_object(player, DBFETCH(thing)->location));
			notify(player, buf);
		}
		/* print thing's actions, if any */
		if (DBFETCH(thing)->exits != NOTHING) {
			notify(player, "Actions/exits:");
			DOLIST(exit, DBFETCH(thing)->exits) {
				notify(player, unparse_object(player, exit));
			}
		} else {
			notify(player, "No actions attached.");
		}
		break;
	case TYPE_PLAYER:

		/* print home */
		sprintf(buf, "Home: %s", unparse_object(player, PLAYER_HOME(thing)));	/* home */
		notify(player, buf);

		/* print location if player can link to it */
		if (DBFETCH(thing)->location != NOTHING && (controls(player, DBFETCH(thing)->location)
													|| can_link_to(player, NOTYPE,
																   DBFETCH(thing)->location))) {
			sprintf(buf, "Location: %s", unparse_object(player, DBFETCH(thing)->location));
			notify(player, buf);
		}
		/* print player's actions, if any */
		if (DBFETCH(thing)->exits != NOTHING) {
			notify(player, "Actions/exits:");
			DOLIST(exit, DBFETCH(thing)->exits) {
				notify(player, unparse_object(player, exit));
			}
		} else {
			notify(player, "No actions attached.");
		}
		break;
	case TYPE_EXIT:
		if (DBFETCH(thing)->location != NOTHING) {
			sprintf(buf, "Source: %s", unparse_object(player, DBFETCH(thing)->location));
			notify(player, buf);
		}
		/* print destinations */
		if (DBFETCH(thing)->sp.exit.ndest == 0)
			break;
		for (i = 0; i < DBFETCH(thing)->sp.exit.ndest; i++) {
			switch ((DBFETCH(thing)->sp.exit.dest)[i]) {
			case NOTHING:
				break;
			case HOME:
				notify(player, "Destination: *HOME*");
				break;
			default:
				sprintf(buf, "Destination: %s",
						unparse_object(player, (DBFETCH(thing)->sp.exit.dest)[i]));
				notify(player, buf);
				break;
			}
		}
		break;
	case TYPE_PROGRAM:
		if (PROGRAM_SIZ(thing)) {
			struct timeval tv = PROGRAM_PROFTIME(thing);
			sprintf(buf, "Program compiled size: %d instructions", PROGRAM_SIZ(thing));
			notify(player, buf);
			sprintf(buf, "Cumulative runtime: %ld.%06ld seconds ", tv.tv_sec, tv.tv_usec);
			notify(player, buf);
		} else {
			sprintf(buf, "Program not compiled.");
			notify(player, buf);
		}

		/* print location if player can link to it */
		if (DBFETCH(thing)->location != NOTHING && (controls(player, DBFETCH(thing)->location)
													|| can_link_to(player, NOTYPE,
																   DBFETCH(thing)->location))) {
			sprintf(buf, "Location: %s", unparse_object(player, DBFETCH(thing)->location));
			notify(player, buf);
		}
		break;
	default:
		/* do nothing */
		break;
	}
}


void
do_score(dbref player)
{
	char buf[BUFFER_LEN];

	sprintf(buf, "You have %d %s.", PLAYER_PENNIES(player),
			PLAYER_PENNIES(player) == 1 ? tp_penny : tp_pennies);
	notify(player, buf);
}

void
do_inventory(dbref player)
{
	dbref thing;

	if ((thing = DBFETCH(player)->contents) == NOTHING) {
		notify(player, "You aren't carrying anything.");
	} else {
		notify(player, "You are carrying:");
		DOLIST(thing, thing) {
			notify(player, unparse_object(player, thing));
		}
	}

	do_score(player);
}

#define UPCASE(x) (toupper(x))

int
init_checkflags(dbref player, const char *flags, struct flgchkdat *check)
{
	char buf[BUFFER_LEN];
	char *cptr;
	int output_type = 0;
	int mode = 0;

	strcpy(buf, flags);
	for (cptr = buf; *cptr && (*cptr != '='); cptr++) ;
	if (*cptr == '=')
		*(cptr++) = '\0';
	flags = buf;
	while (*cptr && isspace(*cptr))
		cptr++;

	if (!*cptr) {
		output_type = 0;
	} else if (string_prefix("owners", cptr)) {
		output_type = 1;
	} else if (string_prefix("locations", cptr)) {
		output_type = 3;
	} else if (string_prefix("links", cptr)) {
		output_type = 2;
	} else if (string_prefix("count", cptr)) {
		output_type = 4;
	} else if (string_prefix("size", cptr)) {
		output_type = 5;
	} else {
		output_type = 0;
	}

	check->fortype = 0;
	check->istype = 0;
	check->isnotroom = 0;
	check->isnotexit = 0;
	check->isnotthing = 0;
	check->isnotplayer = 0;
	check->isnotprog = 0;
	check->setflags = 0;
	check->clearflags = 0;

	check->forlevel = 0;
	check->islevel = 0;
	check->isnotzero = 0;
	check->isnotone = 0;
	check->isnottwo = 0;
	check->isnotthree = 0;

	check->forlink = 0;
	check->islinked = 0;
	check->forold = 0;
	check->isold = 0;

	check->loadedsize = 0;
	check->issize = 0;
	check->size = 0;

	while (*flags) {
		switch (UPCASE(*flags)) {
		case '!':
			if (mode)
				mode = 0;
			else
				mode = 2;
			break;
		case 'R':
			if (mode) {
				check->isnotroom = 1;
			} else {
				check->fortype = 1;
				check->istype = TYPE_ROOM;
			}
			break;
		case 'T':
			if (mode) {
				check->isnotthing = 1;
			} else {
				check->fortype = 1;
				check->istype = TYPE_THING;
			}
			break;
		case 'E':
			if (mode) {
				check->isnotexit = 1;
			} else {
				check->fortype = 1;
				check->istype = TYPE_EXIT;
			}
			break;
		case 'P':
			if (mode) {
				check->isnotplayer = 1;
			} else {
				check->fortype = 1;
				check->istype = TYPE_PLAYER;
			}
			break;
		case 'F':
			if (mode) {
				check->isnotprog = 1;
			} else {
				check->fortype = 1;
				check->istype = TYPE_PROGRAM;
			}
			break;
		case '~':
		case '^':
			check->loadedsize = (Wizard(player) && *flags == '^');
			check->size = atoi(flags + 1);
			check->issize = !mode;
			while (isdigit(flags[1]))
				flags++;
			break;
		case 'U':
			check->forlink = 1;
			if (mode) {
				check->islinked = 1;
			} else {
				check->islinked = 0;
			}
			break;
		case '@':
			check->forold = 1;
			if (mode) {
				check->isold = 0;
			} else {
				check->isold = 1;
			}
			break;
		case '0':
			if (mode) {
				check->isnotzero = 1;
			} else {
				check->forlevel = 1;
				check->islevel = 0;
			}
			break;
		case '1':
			if (mode) {
				check->isnotone = 1;
			} else {
				check->forlevel = 1;
				check->islevel = 1;
			}
			break;
		case '2':
			if (mode) {
				check->isnottwo = 1;
			} else {
				check->forlevel = 1;
				check->islevel = 2;
			}
			break;
		case '3':
			if (mode) {
				check->isnotthree = 1;
			} else {
				check->forlevel = 1;
				check->islevel = 3;
			}
			break;
		case 'M':
			if (mode) {
				check->forlevel = 1;
				check->islevel = 0;
			} else {
				check->isnotzero = 1;
			}
			break;
		case 'A':
			if (mode)
				check->clearflags |= ABODE;
			else
				check->setflags |= ABODE;
			break;
		case 'B':
			if (mode)
				check->clearflags |= BUILDER;
			else
				check->setflags |= BUILDER;
			break;
		case 'C':
			if (mode)
				check->clearflags |= CHOWN_OK;
			else
				check->setflags |= CHOWN_OK;
			break;
		case 'D':
			if (mode)
				check->clearflags |= DARK;
			else
				check->setflags |= DARK;
			break;
		case 'H':
			if (mode)
				check->clearflags |= HAVEN;
			else
				check->setflags |= HAVEN;
			break;
		case 'J':
			if (mode)
				check->clearflags |= JUMP_OK;
			else
				check->setflags |= JUMP_OK;
			break;
		case 'K':
			if (mode)
				check->clearflags |= KILL_OK;
			else
				check->setflags |= KILL_OK;
			break;
		case 'L':
			if (mode)
				check->clearflags |= LINK_OK;
			else
				check->setflags |= LINK_OK;
			break;
		case 'Q':
			if (mode)
				check->clearflags |= QUELL;
			else
				check->setflags |= QUELL;
			break;
		case 'S':
			if (mode)
				check->clearflags |= STICKY;
			else
				check->setflags |= STICKY;
			break;
		case 'V':
			if (mode)
				check->clearflags |= VEHICLE;
			else
				check->setflags |= VEHICLE;
			break;
		case 'Z':
			if (mode)
				check->clearflags |= ZOMBIE;
			else
				check->setflags |= ZOMBIE;
			break;
		case 'W':
			if (mode)
				check->clearflags |= WIZARD;
			else
				check->setflags |= WIZARD;
			break;
		case 'X':
			if (mode)
				check->clearflags |= XFORCIBLE;
			else
				check->setflags |= XFORCIBLE;
			break;
		case ' ':
			if (mode)
				mode = 2;
			break;
		}
		if (mode)
			mode--;
		flags++;
	}
	return output_type;
}


int
checkflags(dbref what, struct flgchkdat check)
{
	if (check.fortype && (Typeof(what) != check.istype))
		return (0);
	if (check.isnotroom && (Typeof(what) == TYPE_ROOM))
		return (0);
	if (check.isnotexit && (Typeof(what) == TYPE_EXIT))
		return (0);
	if (check.isnotthing && (Typeof(what) == TYPE_THING))
		return (0);
	if (check.isnotplayer && (Typeof(what) == TYPE_PLAYER))
		return (0);
	if (check.isnotprog && (Typeof(what) == TYPE_PROGRAM))
		return (0);

	if (check.forlevel && (MLevRaw(what) != check.islevel))
		return (0);
	if (check.isnotzero && (MLevRaw(what) == 0))
		return (0);
	if (check.isnotone && (MLevRaw(what) == 1))
		return (0);
	if (check.isnottwo && (MLevRaw(what) == 2))
		return (0);
	if (check.isnotthree && (MLevRaw(what) == 3))
		return (0);

	if (FLAGS(what) & check.clearflags)
		return (0);
	if ((~FLAGS(what)) & check.setflags)
		return (0);

	if (check.forlink) {
		switch (Typeof(what)) {
		case TYPE_ROOM:
			if ((DBFETCH(what)->sp.room.dropto == NOTHING) != (!check.islinked))
				return (0);
			break;
		case TYPE_EXIT:
			if ((!DBFETCH(what)->sp.exit.ndest) != (!check.islinked))
				return (0);
			break;
		case TYPE_PLAYER:
		case TYPE_THING:
			if (!check.islinked)
				return (0);
			break;
		default:
			if (check.islinked)
				return (0);
		}
	}
	if (check.forold) {
		if (((((time(NULL)) - DBFETCH(what)->ts.lastused) < tp_aging_time) ||
			 (((time(NULL)) - DBFETCH(what)->ts.modified) < tp_aging_time))
			!= (!check.isold))
			return (0);
	}
	if (check.size) {
		if ((size_object(what, check.loadedsize) < check.size)
			!= (!check.issize)) {
			return 0;
		}
	}
	return (1);
}


void
display_objinfo(dbref player, dbref obj, int output_type)
{
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];

	strcpy(buf2, unparse_object(player, obj));

	switch (output_type) {
	case 1:					/* owners */
		sprintf(buf, "%-38.512s  %.512s", buf2, unparse_object(player, OWNER(obj)));
		break;
	case 2:					/* links */
		switch (Typeof(obj)) {
		case TYPE_ROOM:
			sprintf(buf, "%-38.512s  %.512s", buf2,
					unparse_object(player, DBFETCH(obj)->sp.room.dropto));
			break;
		case TYPE_EXIT:
			if (DBFETCH(obj)->sp.exit.ndest == 0) {
				sprintf(buf, "%-38.512s  %.512s", buf2, "*UNLINKED*");
				break;
			}
			if (DBFETCH(obj)->sp.exit.ndest > 1) {
				sprintf(buf, "%-38.512s  %.512s", buf2, "*METALINKED*");
				break;
			}
			sprintf(buf, "%-38.512s  %.512s", buf2,
					unparse_object(player, DBFETCH(obj)->sp.exit.dest[0]));
			break;
		case TYPE_PLAYER:
			sprintf(buf, "%-38.512s  %.512s", buf2, unparse_object(player, PLAYER_HOME(obj)));
			break;
		case TYPE_THING:
			sprintf(buf, "%-38.512s  %.512s", buf2, unparse_object(player, THING_HOME(obj)));
			break;
		default:
			sprintf(buf, "%-38.512s  %.512s", buf2, "N/A");
			break;
		}
		break;
	case 3:					/* locations */
		sprintf(buf, "%-38.512s  %.512s", buf2,
				unparse_object(player, DBFETCH(obj)->location));
		break;
	case 4:
		return;
	case 5:
		sprintf(buf, "%-38.512s  %ld bytes.", buf2, size_object(obj, 0));
		break;
	case 0:
	default:
		strcpy(buf, buf2);
		break;
	}
	notify(player, buf);
}


void
do_find(dbref player, const char *name, const char *flags)
{
	dbref i;
	struct flgchkdat check;
	char buf[BUFFER_LEN + 2];
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	strcpy(buf, "*");
	strcat(buf, name);
	strcat(buf, "*");

	if (!payfor(player, tp_lookup_cost)) {
		notify_fmt(player, "You don't have enough %s.", tp_pennies);
	} else {
		for (i = 0; i < db_top; i++) {
			if ((Wizard(OWNER(player)) || OWNER(i) == OWNER(player)) &&
				checkflags(i, check) && NAME(i) && (!*name || equalstr(buf, (char *) NAME(i)))) {
				display_objinfo(player, i, output_type);
				total++;
			}
		}
		notify(player, "***End of List***");
		notify_fmt(player, "%d objects found.", total);
	}
}


void
do_owned(dbref player, const char *name, const char *flags)
{
	dbref victim, i;
	struct flgchkdat check;
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	if (!payfor(player, tp_lookup_cost)) {
		notify_fmt(player, "You don't have enough %s.", tp_pennies);
		return;
	}
	if (Wizard(OWNER(player)) && *name) {
		if ((victim = lookup_player(name)) == NOTHING) {
			notify(player, "I couldn't find that player.");
			return;
		}
	} else
		victim = player;

	for (i = 0; i < db_top; i++) {
		if ((OWNER(i) == OWNER(victim)) && checkflags(i, check)) {
			display_objinfo(player, i, output_type);
			total++;
		}
	}
	notify(player, "***End of List***");
	notify_fmt(player, "%d objects found.", total);
}

void
do_trace(int descr, dbref player, const char *name, int depth)
{
	dbref thing;
	int i;
	struct match_data md;

	init_match(descr, player, name, NOTYPE, &md);
	match_absolute(&md);
	match_here(&md);
	match_me(&md);
	match_neighbor(&md);
	match_possession(&md);
	match_registered(&md);
	if ((thing = noisy_match_result(&md)) == NOTHING || thing == AMBIGUOUS)
		return;

	for (i = 0; (!depth || i < depth) && thing != NOTHING; i++) {
		if (controls(player, thing) || can_link_to(player, NOTYPE, thing))
			notify(player, unparse_object(player, thing));
		else
			notify(player, "**Missing**");
		thing = DBFETCH(thing)->location;
	}
	notify(player, "***End of List***");
}

void
do_entrances(int descr, dbref player, const char *name, const char *flags)
{
	dbref i, j;
	dbref thing;
	struct match_data md;
	struct flgchkdat check;
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	if (*name == '\0') {
		thing = getloc(player);
	} else {
		init_match(descr, player, name, NOTYPE, &md);
		match_all_exits(&md);
		match_neighbor(&md);
		match_possession(&md);
		match_registered(&md);
		if (Wizard(OWNER(player))) {
			match_absolute(&md);
			match_player(&md);
		}
		match_here(&md);
		match_me(&md);

		thing = noisy_match_result(&md);
	}
	if (thing == NOTHING) {
		notify(player, "I don't know what object you mean.");
		return;
	}
	if (!controls(OWNER(player), thing)) {
		notify(player, "Permission denied.");
		return;
	}
	init_checkflags(player, flags, &check);
	for (i = 0; i < db_top; i++) {
		if (checkflags(i, check)) {
			switch (Typeof(i)) {
			case TYPE_EXIT:
				for (j = DBFETCH(i)->sp.exit.ndest; j--;) {
					if (DBFETCH(i)->sp.exit.dest[j] == thing) {
						display_objinfo(player, i, output_type);
						total++;
					}
				}
				break;
			case TYPE_PLAYER:
				if (PLAYER_HOME(i) == thing) {
					display_objinfo(player, i, output_type);
					total++;
				}
				break;
			case TYPE_THING:
				if (THING_HOME(i) == thing) {
					display_objinfo(player, i, output_type);
					total++;
				}
				break;
			case TYPE_ROOM:
				if (DBFETCH(i)->sp.room.dropto == thing) {
					display_objinfo(player, i, output_type);
					total++;
				}
				break;
			case TYPE_PROGRAM:
			case TYPE_GARBAGE:
				break;
			}
		}
	}
	notify(player, "***End of List***");
	notify_fmt(player, "%d objects found.", total);
}

void
do_contents(int descr, dbref player, const char *name, const char *flags)
{
	dbref i;
	dbref thing;
	struct match_data md;
	struct flgchkdat check;
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	if (*name == '\0') {
		thing = getloc(player);
	} else {
		init_match(descr, player, name, NOTYPE, &md);
		match_me(&md);
		match_here(&md);
		match_all_exits(&md);
		match_neighbor(&md);
		match_possession(&md);
		match_registered(&md);
		if (Wizard(OWNER(player))) {
			match_absolute(&md);
			match_player(&md);
		}

		thing = noisy_match_result(&md);
	}
	if (thing == NOTHING)
		return;
	if (!controls(OWNER(player), thing)) {
		notify(player, "Permission denied.");
		return;
	}
	init_checkflags(player, flags, &check);
	DOLIST(i, DBFETCH(thing)->contents) {
		if (checkflags(i, check)) {
			display_objinfo(player, i, output_type);
			total++;
		}
	}
	switch (Typeof(thing)) {
	case TYPE_EXIT:
	case TYPE_PROGRAM:
	case TYPE_GARBAGE:
		i = NOTHING;
		break;
	case TYPE_ROOM:
	case TYPE_THING:
	case TYPE_PLAYER:
		i = DBFETCH(thing)->exits;
		break;
	}
	DOLIST(i, i) {
		if (checkflags(i, check)) {
			display_objinfo(player, i, output_type);
			total++;
		}
	}
	notify(player, "***End of List***");
	notify_fmt(player, "%d objects found.", total);
}

static int
exit_matches_name(dbref exit, const char *name)
{
	char buf[BUFFER_LEN];
	char *ptr, *ptr2;

	strcpy(buf, NAME(exit));
	for (ptr2 = ptr = buf; *ptr; ptr = ptr2) {
		while (*ptr2 && *ptr2 != ';')
			ptr2++;
		if (*ptr2)
			*ptr2++ = '\0';
		while (*ptr2 == ';')
			ptr2++;
		if (string_prefix(name, ptr) && DBFETCH(exit)->sp.exit.ndest &&
			Typeof((DBFETCH(exit)->sp.exit.dest)[0]) == TYPE_PROGRAM)
			return 1;
	}
	return 0;
}

void
exit_match_exists(dbref player, dbref obj, const char *name)
{
	dbref exit;
	char buf[BUFFER_LEN];

	exit = DBFETCH(obj)->exits;
	while (exit != NOTHING) {
		if (exit_matches_name(exit, name)) {
			sprintf(buf, "  %ss are trapped on %.2048s", name, unparse_object(player, obj));
			notify(player, buf);
		}
		exit = DBFETCH(exit)->next;
	}
}

void
do_sweep(int descr, dbref player, const char *name)
{
	dbref thing, ref, loc;
	int flag, tellflag;
	struct match_data md;
	char buf[BUFFER_LEN];

	if (*name == '\0') {
		thing = getloc(player);
	} else {
		init_match(descr, player, name, NOTYPE, &md);
		match_me(&md);
		match_here(&md);
		match_all_exits(&md);
		match_neighbor(&md);
		match_possession(&md);
		match_registered(&md);
		if (Wizard(OWNER(player))) {
			match_absolute(&md);
			match_player(&md);
		}
		thing = noisy_match_result(&md);
	}
	if (thing == NOTHING) {
		notify(player, "I don't know what object you mean.");
		return;
	}

	if (*name && !controls(OWNER(player), thing)) {
		notify(player, "Permission denied.");
		return;
	}

	sprintf(buf, "Listeners in %s:", unparse_object(player, thing));
	notify(player, buf);

	ref = DBFETCH(thing)->contents;
	for (; ref != NOTHING; ref = DBFETCH(ref)->next) {
		switch (Typeof(ref)) {
		case TYPE_PLAYER:
			if (!Dark(thing) || online(ref)) {
				sprintf(buf, "  %s is a %splayer.",
						unparse_object(player, ref), online(ref) ? "" : "sleeping ");
				notify(player, buf);
			}
			break;
		case TYPE_THING:
			if (FLAGS(ref) & (ZOMBIE | LISTENER)) {
				tellflag = 0;
				sprintf(buf, "  %.255s is a", unparse_object(player, ref));
				if (FLAGS(ref) & ZOMBIE) {
					tellflag = 1;
					if (!online(OWNER(ref))) {
						tellflag = 0;
						strcat(buf, " sleeping");
					}
					strcat(buf, " zombie");
				}
				if ((FLAGS(ref) & LISTENER) &&
					(get_property(ref, "_listen") ||
					 get_property(ref, "~listen") || get_property(ref, "~olisten"))) {
					strcat(buf, " listener");
					tellflag = 1;
				}
				strcat(buf, " object owned by ");
				strcat(buf, unparse_object(player, OWNER(ref)));
				strcat(buf, ".");
				if (tellflag)
					notify(player, buf);
			}
			exit_match_exists(player, ref, "page");
			exit_match_exists(player, ref, "whisper");
			exit_match_exists(player, ref, "pose");
			exit_match_exists(player, ref, "say");
			break;
		}
	}
	flag = 0;
	loc = thing;
	while (loc != NOTHING) {
		if (controls(player, loc)) {
			if (!flag) {
				notify(player, "Listening rooms down the environment:");
				flag = 1;
			}

			if ((FLAGS(loc) & LISTENER) &&
				(get_property(loc, "_listen") ||
				 get_property(loc, "~listen") || get_property(loc, "~olisten"))) {
				sprintf(buf, "  %s is a listening room.", unparse_object(player, loc));
				notify(player, buf);
			}

			exit_match_exists(player, loc, "page");
			exit_match_exists(player, loc, "whisper");
			exit_match_exists(player, loc, "pose");
			exit_match_exists(player, loc, "say");
		}
		loc = getparent(loc);
	}
	notify(player, "**End of list**");
}
