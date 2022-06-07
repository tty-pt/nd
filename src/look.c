
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
#include "view.h"

#define EXEC_SIGNAL '@'			/* Symbol which tells us what we're looking at
					 * is an execution order and not a message.    */

/* prints owner of something */
static void
print_owner(dbref player, dbref thing)
{
	char buf[BUFFER_LEN];

	switch (Typeof(thing)) {
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%s is an entity.", NAME(thing));
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
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
	if (db[loc].contents > 0) {
                DOLIST(thing, db[loc].contents) {
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
		art(ENTITY(player)->fd, art_str);

	if (GETDESC(thing)) {
		notify(player, GETDESC(thing));
	} else if (!art_str) {
		notify(player, "You see nothing special.");
	}
}

void
look_room(dbref player, dbref loc)
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
	look_room(player, loc);
}

void
do_look_at(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref thing;

	if (*name == '\0' || !strcmp(name, "here")) {
		thing = getloc(player);
	} else if (
			(thing = ematch_absolute(name)) == NOTHING
			&& (thing = ematch_here(player, name)) == NOTHING
			&& (thing = ematch_me(player, name)) == NOTHING
			&& (thing = ematch_near(player, name)) == NOTHING
			&& (thing = ematch_mine(player, name)) == NOTHING
		  )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	switch (Typeof(thing)) {
	case TYPE_ROOM:
		look_room(player, thing);
		view(player);
		break;
	case TYPE_ENTITY:
		if (web_look(player, thing, GETDESC(thing))) {
			look_simple(player, thing);
			look_contents(player, thing, "Carrying:");
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (web_look(player, thing, GETDESC(thing))) {
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

	if (Typeof(i) == TYPE_EXIT && db[i].sp.exit.dest) {
		byts += sizeof(dbref) * db[i].sp.exit.ndest;
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

	if (*name == '\0') {
		if ((thing = getloc(player)) == NOTHING)
			return;
	} else if ((thing = ematch_all(player, name)) == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
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
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Parent: ",
				(int) (BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				thing,
				NAME(OWNER(thing)));
		strlcat(buf, unparse_object(player, db[thing].location), sizeof(buf));
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Value: %d",
				(int) (BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				thing,
				NAME(OWNER(thing)), GETVALUE(thing));
		break;
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%.*s (#%d) %s: %d  ", 
				(int) (BUFFER_LEN - strlen(CPENNIES) - 35),
				unparse_object(player, thing),
				thing,
				CPENNIES, GETVALUE(thing));
		break;
	case TYPE_EXIT:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s",
				(int) (BUFFER_LEN - strlen(NAME(OWNER(thing))) - 35),
				unparse_object(player, thing),
				thing,
				NAME(OWNER(thing)));
		break;
	case TYPE_GARBAGE:
		strlcpy(buf, unparse_object(player, thing), sizeof(buf));
		break;
	}
	notify(player, buf);

	if (GETDESC(thing))
		notify(player, GETDESC(thing));

	notify(player, "[ Use 'examine <object>=/' to list root properties. ]");

	snprintf(buf, sizeof(buf), "Memory used: %ld bytes", size_object(thing, 1));
	notify(player, buf);

	/* show him the contents */
	if (db[thing].contents != NOTHING) {
		notify(player, "Contents:");
		DOLIST(content, db[thing].contents) {
			notify(player, unparse_object(player, content));
		}
	}
	switch (Typeof(thing)) {
	case TYPE_ROOM:
		/* tell him about exits */
		if (db[thing].sp.room.exits != NOTHING) {
			notify(player, "Exits:");
			DOLIST(exit, db[thing].sp.room.exits) {
				notify(player, unparse_object(player, exit));
			}
		} else {
			notify(player, "No exits.");
		}

		/* print dropto if present */
		if (db[thing].sp.room.dropto != NOTHING) {
			snprintf(buf, sizeof(buf), "Dropped objects go to: %s",
					unparse_object(player, db[thing].sp.room.dropto));
			notify(player, buf);
		}
		break;
	case TYPE_EQUIPMENT:
		notifyf(player, "equipment eqw %u msv %u.", EQUIPMENT(thing)->eqw, EQUIPMENT(thing)->msv);
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (db[thing].location != NOTHING && controls(player, db[thing].location)) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse_object(player, db[thing].location));
			notify(player, buf);
		}
		break;
	case TYPE_ENTITY:

		/* print home */
		snprintf(buf, sizeof(buf), "Home: %s", unparse_object(player, ENTITY(thing)->home));	/* home */
		notify(player, buf);
		notifyf(player, "hp: %d/%d entity flags: %d", ENTITY(thing)->hp, HP_MAX(thing), ENTITY(thing)->flags);

		/* print location if player can link to it */
		if (db[thing].location != NOTHING && controls(player, db[thing].location)) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse_object(player, db[thing].location));
			notify(player, buf);
		}
		break;
	case TYPE_EXIT:
		if (db[thing].location != NOTHING) {
			snprintf(buf, sizeof(buf), "Source: %s", unparse_object(player, db[thing].location));
			notify(player, buf);
		}
		/* print destinations */
		if (db[thing].sp.exit.ndest == 0)
			break;
		for (i = 0; i < db[thing].sp.exit.ndest; i++) {
			switch ((db[thing].sp.exit.dest)[i]) {
			case NOTHING:
				break;
			case HOME:
				notify(player, "Destination: *HOME*");
				break;
			default:
				snprintf(buf, sizeof(buf), "Destination: %s",
						unparse_object(player, (db[thing].sp.exit.dest)[i]));
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
                if ((thing = db[player].contents) == NOTHING) {
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
					unparse_object(player, db[obj].sp.room.dropto));
			break;
		case TYPE_EXIT:
			if (db[obj].sp.exit.ndest == 0) {
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "*UNLINKED*");
				break;
			}
			if (db[obj].sp.exit.ndest > 1) {
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "*METALINKED*");
				break;
			}
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
					unparse_object(player, db[obj].sp.exit.dest[0]));
			break;
		case TYPE_ENTITY:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, ENTITY(obj)->home));
			break;
		default:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "N/A");
			break;
		}
		break;
	case 3:					/* locations */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
				unparse_object(player, db[obj].location));
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
	dbref i;
	char buf[BUFFER_LEN + 2];
	int total = 0;

	strlcpy(buf, "*", sizeof(buf));
	strlcat(buf, name, sizeof(buf));
	strlcat(buf, "*", sizeof(buf));

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(player, "You don't have enough %s.", PENNIES);
	} else {
		for (i = 0; i < db_top; i++) {
			if ((Wizard(OWNER(player)) || OWNER(i) == OWNER(player)) &&
				NAME(i) && (!*name || equalstr(buf, (char *) NAME(i)))) {
				display_objinfo(player, i, 0);
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
	dbref victim, i;
	int total = 0;

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
		if (OWNER(i) == OWNER(victim)) {
			display_objinfo(player, i, 0);
			total++;
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
	dbref i;
	dbref thing;
	int total = 0;

	if (*name == '\0') {
		thing = getloc(player);
	} else if ((thing = ematch_all(player, name)) == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(OWNER(player), thing)) {
		notify(player, "Permission denied. (You can't get the contents of something you don't control)");
		return;
	}
	DOLIST(i, db[thing].contents) {
		display_objinfo(player, i, 0);
		total++;
	}
	switch (Typeof(thing)) {
	case TYPE_EXIT:
	case TYPE_GARBAGE:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_ENTITY:
		i = NOTHING;
		break;
	case TYPE_ROOM:
		i = db[thing].sp.room.exits;
		break;
	}
	DOLIST(i, i) {
		display_objinfo(player, i, 0);
		total++;
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
	char buf[BUFFER_LEN];

	if (*name == '\0') {
		thing = getloc(player);
	} else if ((thing = ematch_all(player, name)) == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (*name && !controls(OWNER(player), thing)) {
		notify(player, "Permission denied. (You can't perform a security sweep in a room you don't own)");
		return;
	}

	snprintf(buf, sizeof(buf), "Listeners in %s:", unparse_object(player, thing));
	notify(player, buf);

	ref = db[thing].contents;
	for (; ref != NOTHING; ref = db[ref].next) {
		switch (Typeof(ref)) {
		case TYPE_ENTITY:
			if (!Dark(thing)) {
				snprintf(buf, sizeof(buf), "  %s is a %splayer.",
						unparse_object(player, ref), ENTITY(ref)->fd > 0 ? "" : "sleeping ");
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
