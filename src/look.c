
/* $Header$ */

#include "copyright.h"
#include "config.h"
#include "params.h"

/* commands which look at things */

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "defaults.h"
#include "props.h"
#include "interface.h"
#include "match.h"
#include "dbsearch.h"
#include "externs.h"
#include "web.h"

#define EXEC_SIGNAL '@'			/* Symbol which tells us what we're looking at
					 * is an execution order and not a message.    */

/* prints owner of something */
static void
print_owner(dbref player, dbref thing)
{
	char buf[BUFFER_LEN];

	switch (Typeof(thing)) {
	case TYPE_PLAYER:
		snprintf(buf, sizeof(buf), "%s is a player.", NAME(thing));
		break;
	case TYPE_ROOM:
	case TYPE_THING:
	case TYPE_EXIT:
		snprintf(buf, sizeof(buf), "Owner: %s", NAME(OWNER(thing)));
		break;
	case TYPE_GARBAGE:
		snprintf(buf, sizeof(buf), "%s is garbage.", NAME(thing));
		break;
	}
	notify(player, buf);
}

static void
look_contents(dbref player, dbref loc, const char *contents_name)
{
        char buf[BUFSIZ];
        size_t buf_l = 0;
	dbref thing;
	dbref can_see_loc;

	/* check to see if he can see the location */
	can_see_loc = (!Dark(loc) || controls(player, loc));

	/* check to see if there is anything there */
	if (DBFETCH(loc)->contents > 0) {
                DOLIST(thing, DBFETCH(loc)->contents) {
                        if (can_see(player, thing, can_see_loc)) {
                                buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
                                         "\n%s", unparse_object(player, thing));
                        }
                }
	}

        buf[buf_l] = '\0';

        notifyf(player, "%s%s", contents_name, buf);
}

extern void art(dbref descr, const char *arts);

static void
look_simple(dbref player, dbref thing)
{
	char const *art_str = GETMESG(thing, MESGPROP_ART);

	if (art_str)
		art(PLAYER_FD(player), art_str);

	if (GETDESC(thing)) {
		notify(player, GETDESC(thing));
	} else if (!art_str) {
		notify(player, "You see nothing special.");
	}
}

void
look_room(dbref player, dbref loc, int verbose)
{
	char const *description = "";
	/* tell him the name, and the number if he can link to it */

	/* tell him the description */
	if (Typeof(loc) == TYPE_ROOM) {
		if (GETDESC(loc)) {
			description = GETDESC(loc);
		}
		/* tell him the appropriate messages if he has the key */
		can_doit(player, loc, 0);
	} else {
		if (GETIDESC(loc)) {
			description = GETIDESC(loc);
		}
	}

	if (web_look(player, loc, description)) {
		notify(player, unparse_object(player, loc));
		notify(player, description);
		/* tell him the contents */
		look_contents(player, loc, "You see:");
	}
}

void
look_around(dbref player)
{
	dbref loc;

	if ((loc = getloc(player)) == NOTHING)
		return;
	look_room(player, loc, 1);
}

void
do_look_at(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *detail = cmd->argv[2];
	dbref thing;
	struct match_data md;
	/* int res; */
	char buf[BUFFER_LEN];

	if (*name == '\0' || !strcmp(name, "here")) {
		if ((thing = getloc(player)) != NOTHING) {
			look_room(player, thing, 1);
		}
	} else {
		/* look at a thing here */
		init_match(player, name, &md);
		match_neighbor(&md);
		match_possession(&md);
                match_absolute(&md);
		if (Wizard(OWNER(player))) {
			match_player(&md);
		}
		match_here(&md);
		match_me(&md);

		thing = match_result(&md);

                if (Typeof(thing) != TYPE_ROOM && getloc(thing) != getloc(player)) {
                        notify(player, "That is too far away to see.");
                        return;
                }

		if (thing != NOTHING && thing != AMBIGUOUS && !*detail) {
			switch (Typeof(thing)) {
			case TYPE_ROOM:
				if (getloc(player) != thing && !can_link_to(player, TYPE_ROOM, thing)) {
					notify(player, "Permission denied. (you're not where you want to look, and can't link to it)");
				} else {
					look_room(player, thing, 1);
				}
				break;
			case TYPE_PLAYER:
				if (getloc(player) != getloc(thing)
					&& !controls(player, thing)) {
					notify(player, "Permission denied. (Your location isn't the same as what you're looking at)");
				} else if (web_look(player, thing, GETDESC(thing))) {
					look_simple(player, thing);
					look_contents(player, thing, "Carrying:");
				}
				break;
			case TYPE_THING:
				if (getloc(player) != getloc(thing)
					&& getloc(thing) != player && !controls(player, thing)) {
					notify(player, "Permission denied. (You're not in the same room as or carrying the object)");
				} else if (web_look(player, thing, GETDESC(thing))) {
					look_simple(player, thing);
					if (!(FLAGS(thing) & HAVEN)) {
						look_contents(player, thing, "Contains:");
					}
				}
				break;
			default:
				if (web_look(player, thing, GETDESC(thing)))
                                        look_simple(player, thing);
				break;
			}
		} else if (thing == NOTHING || (*detail && thing != AMBIGUOUS)) {
			int ambig_flag = 0;
			char propname[BUFFER_LEN];
			PropPtr propadr, pptr, lastmatch = NULL;

			if (thing == NOTHING) {
				thing = getloc(player);
				snprintf(buf, sizeof(buf), "%s", name);
			} else {
				snprintf(buf, sizeof(buf), "%s", detail);
			}


			lastmatch = NULL;
			propadr = first_prop(thing, "_details/", &pptr, propname, sizeof(propname));
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
				propadr = next_prop(pptr, propadr, propname, sizeof(propname));
			}
			if (lastmatch && PropType(lastmatch) == PROP_STRTYP) {
				notify(player, PropDataStr(lastmatch));
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

	strlcpy(wld, wild, sizeof(wld));
	i = strlen(wld);
	if (i && wld[i - 1] == PROPDIR_DELIMITER)
		strlcat(wld, "*", sizeof(wld));
	for (wldcrd = wld; *wldcrd == PROPDIR_DELIMITER; wldcrd++) ;
	if (!strcmp(wldcrd, "**"))
		recurse = 1;

	for (ptr = wldcrd; *ptr && *ptr != PROPDIR_DELIMITER; ptr++) ;
	if (*ptr)
		*ptr++ = '\0';

	propadr = first_prop(thing, (char *) dir, &pptr, propname, sizeof(propname));
	while (propadr) {
		if (equalstr(wldcrd, propname)) {
			snprintf(buf, sizeof(buf), "%s%c%s", dir, PROPDIR_DELIMITER, propname);
			if (!Prop_System(buf) && ((!Prop_Hidden(buf) && !(PropFlags(propadr) & PROP_SYSPERMS))
				|| Wizard(OWNER(player)))) {
				if (!*ptr || recurse) {
					cnt++;
					displayprop(player, thing, buf, buf2, sizeof(buf2));
					notify(player, buf2);
				}
				if (recurse)
					ptr = "**";
				cnt += listprops_wildcard(player, thing, buf, ptr);
			}
		}
		propadr = next_prop(pptr, propadr, propname, sizeof(propname));
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
	}
	return byts;
}


void
do_examine(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *dir = cmd->argv[2];
	dbref thing;
	char buf[BUFFER_LEN];
	dbref content;
	dbref exit;
	int i, cnt;
	struct match_data md;

	if (*name == '\0') {
		if ((thing = getloc(player)) == NOTHING)
			return;
	} else {
		/* look it up */
		init_match(player, name, &md);

		match_neighbor(&md);
		match_possession(&md);
		match_absolute(&md);

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
		snprintf(buf, sizeof(buf), "%d propert%s listed.", cnt, (cnt == 1 ? "y" : "ies"));
		notify(player, buf);
		return;
	}
	switch (Typeof(thing)) {
	case TYPE_ROOM:
		snprintf(buf, sizeof(buf), "%.*s  Owner: %s  Parent: ",
				(int) (BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				NAME(OWNER(thing)));
		strlcat(buf, unparse_object(player, DBFETCH(thing)->location), sizeof(buf));
		break;
	case TYPE_THING:
		snprintf(buf, sizeof(buf), "%.*s  Owner: %s  Value: %d",
				(int) (BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				NAME(OWNER(thing)), GETVALUE(thing));
		break;
	case TYPE_PLAYER:
		snprintf(buf, sizeof(buf), "%.*s  %s: %d  ", 
				(int) (BUFFER_LEN - strlen(CPENNIES) - 35),
				unparse_object(player, thing),
				CPENNIES, GETVALUE(thing));
		break;
	case TYPE_EXIT:
		snprintf(buf, sizeof(buf), "%.*s  Owner: %s",
				(int) (BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				NAME(OWNER(thing)));
		break;
	case TYPE_GARBAGE:
		strlcpy(buf, unparse_object(player, thing), sizeof(buf));
		break;
	}
	notify(player, buf);

	if (GETDESC(thing))
		notify(player, GETDESC(thing));
	snprintf(buf, sizeof(buf), "Key: %s", unparse_boolexp(player, GETLOCK(thing), 1));
	notify(player, buf);

	snprintf(buf, sizeof(buf), "Chown_OK Key: %s",
			unparse_boolexp(player, get_property_lock(thing, "_/chlk"), 1));
	notify(player, buf);

	snprintf(buf, sizeof(buf), "Container Key: %s",
			unparse_boolexp(player, get_property_lock(thing, "_/clk"), 1));
	notify(player, buf);

	snprintf(buf, sizeof(buf), "Force Key: %s",
			unparse_boolexp(player, get_property_lock(thing, "@/flk"), 1));
	notify(player, buf);

	if (GETSUCC(thing)) {
		snprintf(buf, sizeof(buf), "Success: %s", GETSUCC(thing));
		notify(player, buf);
	}
	if (GETFAIL(thing)) {
		snprintf(buf, sizeof(buf), "Fail: %s", GETFAIL(thing));
		notify(player, buf);
	}
	if (GETDROP(thing)) {
		snprintf(buf, sizeof(buf), "Drop: %s", GETDROP(thing));
		notify(player, buf);
	}
	if (GETOSUCC(thing)) {
		snprintf(buf, sizeof(buf), "Osuccess: %s", GETOSUCC(thing));
		notify(player, buf);
	}
	if (GETOFAIL(thing)) {
		snprintf(buf, sizeof(buf), "Ofail: %s", GETOFAIL(thing));
		notify(player, buf);
	}
	if (GETODROP(thing)) {
		snprintf(buf, sizeof(buf), "Odrop: %s", GETODROP(thing));
		notify(player, buf);
	}

#if WHO_DOING
	if (GETDOING(thing)) {
		snprintf(buf, sizeof(buf), "Doing: %s", GETDOING(thing));
		notify(player, buf);
	}
#endif
	if (GETOECHO(thing)) {
		snprintf(buf, sizeof(buf), "Oecho: %s", GETOECHO(thing));
		notify(player, buf);
	}
	if (GETPECHO(thing)) {
		snprintf(buf, sizeof(buf), "Pecho: %s", GETPECHO(thing));
		notify(player, buf);
	}
	if (GETIDESC(thing)) {
		snprintf(buf, sizeof(buf), "Idesc: %s", GETIDESC(thing));
		notify(player, buf);
	}

	notify(player, "[ Use 'examine <object>=/' to list root properties. ]");

	snprintf(buf, sizeof(buf), "Memory used: %ld bytes", size_object(thing, 1));
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
			snprintf(buf, sizeof(buf), "Dropped objects go to: %s",
					unparse_object(player, DBFETCH(thing)->sp.room.dropto));
			notify(player, buf);
		}
		break;
	case TYPE_THING:
		/* print home */
		snprintf(buf, sizeof(buf), "Home: %s", unparse_object(player, THING_HOME(thing)));	/* home */
		notify(player, buf);
		/* print location if player can link to it */
		if (DBFETCH(thing)->location != NOTHING && (controls(player, DBFETCH(thing)->location)
													|| can_link_to(player, NOTYPE,
																   DBFETCH(thing)->location))) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse_object(player, DBFETCH(thing)->location));
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
		snprintf(buf, sizeof(buf), "Home: %s", unparse_object(player, PLAYER_HOME(thing)));	/* home */
		notify(player, buf);

		/* print location if player can link to it */
		if (DBFETCH(thing)->location != NOTHING && (controls(player, DBFETCH(thing)->location)
													|| can_link_to(player, NOTYPE,
																   DBFETCH(thing)->location))) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse_object(player, DBFETCH(thing)->location));
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
			snprintf(buf, sizeof(buf), "Source: %s", unparse_object(player, DBFETCH(thing)->location));
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
				snprintf(buf, sizeof(buf), "Destination: %s",
						unparse_object(player, (DBFETCH(thing)->sp.exit.dest)[i]));
				notify(player, buf);
				break;
			}
		}
		break;
	default:
		/* do nothing */
		break;
	}
}


void
do_score(command_t *cmd)
{
	dbref player = cmd->player;
	char buf[BUFFER_LEN];

	snprintf(buf, sizeof(buf), "You have %d %s.", GETVALUE(player),
			GETVALUE(player) == 1 ? PENNY : PENNIES);
	notify(player, buf);
}

void
do_inventory(command_t *cmd)
{
	dbref player = cmd->player;
	dbref thing;

        if (web_look(player, player, GETDESC(player))) {
                if ((thing = DBFETCH(player)->contents) == NOTHING) {
                        notify(player, "You aren't carrying anything.");
                } else {
                        notify(player, "You are carrying:");
                        DOLIST(thing, thing) {
                                notify(player, unparse_object(player, thing));
                        }
                }
        }

	do_score(cmd);
}

int
init_checkflags(dbref player, const char *flags, struct flgchkdat *check)
{
	char buf[BUFFER_LEN];
	char *cptr;
	int output_type = 0;
	int mode = 0;

	strlcpy(buf, flags, sizeof(buf));
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
		switch (toupper(*flags)) {
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
		case 'W':
			if (mode)
				check->clearflags |= WIZARD;
			else
				check->setflags |= WIZARD;
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

	strlcpy(buf2, unparse_object(player, obj), sizeof(buf2));

	switch (output_type) {
	case 1:					/* owners */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, OWNER(obj)));
		break;
	case 2:					/* links */
		switch (Typeof(obj)) {
		case TYPE_ROOM:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
					unparse_object(player, DBFETCH(obj)->sp.room.dropto));
			break;
		case TYPE_EXIT:
			if (DBFETCH(obj)->sp.exit.ndest == 0) {
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "*UNLINKED*");
				break;
			}
			if (DBFETCH(obj)->sp.exit.ndest > 1) {
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "*METALINKED*");
				break;
			}
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
					unparse_object(player, DBFETCH(obj)->sp.exit.dest[0]));
			break;
		case TYPE_PLAYER:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, PLAYER_HOME(obj)));
			break;
		case TYPE_THING:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, THING_HOME(obj)));
			break;
		default:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "N/A");
			break;
		}
		break;
	case 3:					/* locations */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
				unparse_object(player, DBFETCH(obj)->location));
		break;
	case 4:
		return;
	case 5:
		snprintf(buf, sizeof(buf), "%-38.512s  %ld bytes.", buf2, size_object(obj, 0));
		break;
	case 0:
	default:
		strlcpy(buf, buf2, sizeof(buf));
		break;
	}
	notify(player, buf);
}


void
do_find(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *flags = cmd->argv[2];
	dbref i;
	struct flgchkdat check;
	char buf[BUFFER_LEN + 2];
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	strlcpy(buf, "*", sizeof(buf));
	strlcat(buf, name, sizeof(buf));
	strlcat(buf, "*", sizeof(buf));

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(player, "You don't have enough %s.", PENNIES);
	} else {
		for (i = 0; i < db_top; i++) {
			if ((Wizard(OWNER(player)) || OWNER(i) == OWNER(player)) &&
				checkflags(i, check) && NAME(i) && (!*name || equalstr(buf, (char *) NAME(i)))) {
				display_objinfo(player, i, output_type);
				total++;
			}
		}
		notify(player, "***End of List***");
		notifyf(player, "%d objects found.", total);
	}
}


void
do_owned(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *flags = cmd->argv[2];
	dbref victim, i;
	struct flgchkdat check;
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(player, "You don't have enough %s.", PENNIES);
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
	notifyf(player, "%d objects found.", total);
}

void
do_trace(command_t *cmd)
{
	dbref player = cmd->fd;
	const char *name = cmd->argv[1];
	int depth = atoi(cmd->argv[2]);
	dbref thing;
	int i;
	struct match_data md;

	init_match(player, name, &md);
	match_absolute(&md);
	match_here(&md);
	match_me(&md);
	match_neighbor(&md);
	match_possession(&md);
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

void // TODO REMOVE?
do_entrances(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *flags = cmd->argv[2];
	dbref i, j;
	dbref thing;
	struct match_data md;
	struct flgchkdat check;
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	if (*name == '\0') {
		thing = getloc(player);
	} else {
		init_match(player, name, &md);
		match_neighbor(&md);
		match_possession(&md);
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
		notify(player, "Permission denied. (You can't list entrances of objects you don't control)");
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
			case TYPE_GARBAGE:
				break;
			}
		}
	}
	notify(player, "***End of List***");
	notifyf(player, "%d objects found.", total);
}

void
do_contents(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *flags = cmd->argv[2];
	dbref i;
	dbref thing;
	struct match_data md;
	struct flgchkdat check;
	int total = 0;
	int output_type = init_checkflags(player, flags, &check);

	if (*name == '\0') {
		thing = getloc(player);
	} else {
		init_match(player, name, &md);
		match_me(&md);
		match_here(&md);
		match_neighbor(&md);
		match_possession(&md);
		if (Wizard(OWNER(player))) {
			match_absolute(&md);
			match_player(&md);
		}

		thing = noisy_match_result(&md);
	}
	if (thing == NOTHING)
		return;
	if (!controls(OWNER(player), thing)) {
		notify(player, "Permission denied. (You can't get the contents of something you don't control)");
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
	notifyf(player, "%d objects found.", total);
}

void // TODO REMOVE?
do_sweep(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref thing, ref, loc;
	int flag;
	struct match_data md;
	char buf[BUFFER_LEN];

	if (*name == '\0') {
		thing = getloc(player);
	} else {
		init_match(player, name, &md);
		match_me(&md);
		match_here(&md);
		match_neighbor(&md);
		match_possession(&md);
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
		notify(player, "Permission denied. (You can't perform a security sweep in a room you don't own)");
		return;
	}

	snprintf(buf, sizeof(buf), "Listeners in %s:", unparse_object(player, thing));
	notify(player, buf);

	ref = DBFETCH(thing)->contents;
	for (; ref != NOTHING; ref = DBFETCH(ref)->next) {
		switch (Typeof(ref)) {
		case TYPE_PLAYER:
			if (!Dark(thing)) {
				snprintf(buf, sizeof(buf), "  %s is a %splayer.",
						unparse_object(player, ref), PLAYER_SP(ref)->fd > 0 ? "" : "sleeping ");
				notify(player, buf);
			}
			break;
		}
	}
	flag = 0;
	loc = thing;
	while (loc != NOTHING) {
		if (!flag) {
			notify(player, "Listening rooms down the environment:");
			flag = 1;
		}

		loc = getparent(loc);
	}
	notify(player, "**End of list**");
}
static const char *look_c_version = "$RCSfile$ $Revision: 1.29 $";
const char *get_look_c_version(void) { return look_c_version; }
