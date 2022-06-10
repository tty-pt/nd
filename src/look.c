
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

	switch (OBJECT(thing)->type) {
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%s is an entity.", OBJECT(thing)->name);
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		snprintf(buf, sizeof(buf), "Owner: %s", OBJECT(OBJECT(thing)->owner)->name);
		break;
	case TYPE_GARBAGE:
		snprintf(buf, sizeof(buf), "%s is garbage.", OBJECT(thing)->name);
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

	/* check to see if there is anything there */
	if (OBJECT(loc)->contents > 0) {
                DOLIST(thing, OBJECT(loc)->contents) {
			if (thing == player)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"\n%s", unparse_object(player, thing));
                }
	}

        buf[buf_l] = '\0';

        notifyf(player, "%s%s", contents_name, buf);
}

extern void art(dbref descr, const char *arts);

static void
look_simple(dbref player, dbref thing)
{
	char const *art_str = OBJECT(thing)->art;

	if (art_str)
		art(ENTITY(player)->fd, art_str);

	if (OBJECT(thing)->description) {
		notify(player, OBJECT(thing)->description);
	} else if (!art_str) {
		notify(player, "You see nothing special.");
	}
}

void
look_room(dbref player, dbref loc)
{
	char const *description = "";
	/* tell him the name, and the number if he can link to it */

	CBUG(OBJECT(loc)->type != TYPE_ROOM);

	/* tell him the appropriate messages if he has the key */
	can_doit(player, loc, 0);

	if (web_look(player, loc)) {
		notify(player, unparse_object(player, loc));
		notify(player, description);
		/* tell him the contents */
		look_contents(player, loc, "You see:");
	}
}

void
look_around(dbref player)
{
	look_room(player, getloc(player));
}

void
do_look_at(command_t *cmd)
{
	OBJ *player = OBJECT(cmd->player);
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (*name == '\0') {
		thing = OBJECT(player->location);
	} else if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_here(player, name))
			&& !(thing = ematch_me(player, name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
		  )
	{
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		look_room(REF(player), REF(thing));
		view(REF(player));
		break;
	case TYPE_ENTITY:
		if (web_look(REF(player), REF(thing))) {
			look_simple(REF(player), REF(thing));
			look_contents(REF(player), REF(thing), "Carrying:");
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (web_look(REF(player), REF(thing)))
			look_simple(REF(player), REF(thing));
		break;
	default:
		if (web_look(REF(player), REF(thing)))
			look_simple(REF(player), REF(thing));
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
				|| ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD)) {
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

	if (OBJECT(i)->name) {
		byts += strlen(OBJECT(i)->name) + 1;
	}
	byts += size_properties(i, load);

	return byts;
}


void
do_examine(command_t *cmd)
{
	OBJ *player = OBJECT(cmd->player);
	const char *name = cmd->argv[1];
	const char *dir = cmd->argv[2];
	OBJ *thing;
	char buf[BUFFER_LEN];
	dbref content;
	int cnt;

	if (*name == '\0') {
		thing = OBJECT(player->location);
	} else if (!(thing = ematch_all(player, name))) {
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	}

	if (!controls(REF(player), REF(thing))) {
		print_owner(REF(player), REF(thing));
		return;
	}
	if (*dir) {
		/* show him the properties */
		cnt = listprops_wildcard(REF(player), REF(thing), "", dir);
		snprintf(buf, sizeof(buf), "%d propert%s listed.", cnt, (cnt == 1 ? "y" : "ies"));
		notify(REF(player), buf);
		return;
	}
	switch (thing->type) {
	case TYPE_ROOM:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Parent: ",
				(int) (BUFFER_LEN - strlen(OBJECT(thing->owner)->name) - 35),
				unparse_object(REF(player), REF(thing)),
				REF(thing),
				OBJECT(thing->owner)->name);
		strlcat(buf, unparse_object(REF(player), thing->location), sizeof(buf));
		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Value: %d",
				(int) (BUFFER_LEN - strlen(OBJECT(thing->owner)->name) - 35),
				unparse_object(REF(player), REF(thing)),
				REF(thing),
				OBJECT(thing->owner)->name, thing->value);
		break;
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%.*s (#%d) %s: %d  ", 
				(int) (BUFFER_LEN - strlen(CPENNIES) - 35),
				unparse_object(REF(player), REF(thing)),
				REF(thing),
				CPENNIES, thing->value);
		break;
	case TYPE_GARBAGE:
		strlcpy(buf, unparse_object(REF(player), REF(thing)), sizeof(buf));
		break;
	}
	notify(REF(player), buf);

	if (thing->description)
		notify(REF(player), thing->description);

	notify(REF(player), "[ Use 'examine <object>=/' to list root properties. ]");

	snprintf(buf, sizeof(buf), "Memory used: %ld bytes", size_object(REF(thing), 1));
	notify(REF(player), buf);

	/* show him the contents */
	if (thing->contents != NOTHING) {
		notify(REF(player), "Contents:");
		DOLIST(content, thing->contents) {
			notify(REF(player), unparse_object(REF(player), content));
		}
	}
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			notifyf(REF(player), "Exits: %hhx Doors: %hhx", rthing->exits, rthing->doors);

			/* print dropto if present */
			if (rthing->dropto != NOTHING) {
				snprintf(buf, sizeof(buf), "Dropped objects go to: %s",
						unparse_object(REF(player), rthing->dropto));
				notify(REF(player), buf);
			}
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *ething = &thing->sp.equipment;
			notifyf(REF(player), "equipment eqw %u msv %u.", ething->eqw, ething->msv);
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pthing = &thing->sp.plant;
			notifyf(REF(player), "plant plid %u size %u.", pthing->plid, pthing->size);
		}
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (thing->location != NOTHING && controls(REF(player), thing->location)) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse_object(REF(player), thing->location));
			notify(REF(player), buf);
		}
		break;
	case TYPE_ENTITY:
		{
			ENT *ething = &thing->sp.entity;

			snprintf(buf, sizeof(buf), "Home: %s", unparse_object(REF(player), ething->home));
			notify(REF(player), buf);
			notifyf(REF(player), "hp: %d/%d entity flags: %d", ething->hp, HP_MAX(REF(thing)), ething->flags);

			/* print location if player can link to it */
			if (thing->location != NOTHING && controls(REF(player), thing->location)) {
				snprintf(buf, sizeof(buf), "Location: %s", unparse_object(REF(player), thing->location));
				notify(REF(player), buf);
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
	OBJ *o = OBJECT(player);
	char buf[BUFFER_LEN];

	snprintf(buf, sizeof(buf), "You have %d %s.", o->value,
			o->value == 1 ? PENNY : PENNIES);
	notify(player, buf);
}

void
do_inventory(command_t *cmd)
{
	dbref player = cmd->player;
	dbref thing;

        if (web_look(player, player)) {
                if ((thing = OBJECT(player)->contents) == NOTHING) {
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
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, OBJECT(obj)->owner));
		break;
	case 2:					/* links */
		switch (OBJECT(obj)->type) {
		case TYPE_ROOM:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
					unparse_object(player, ROOM(obj)->dropto));
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
				unparse_object(player, OBJECT(obj)->location));
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
			if (((ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD) || OBJECT(i)->owner == OBJECT(player)->owner) &&
				OBJECT(i)->name && (!*name || equalstr(buf, (char *) OBJECT(i)->name))) {
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
	if ((ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD) && *name) {
		if ((victim = lookup_player(name)) == NOTHING) {
			notify(player, "I couldn't find that player.");
			return;
		}
	} else
		victim = player;

	for (i = 0; i < db_top; i++) {
		if (OBJECT(i)->owner == OBJECT(victim)->owner) {
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
	OBJ *player = OBJECT(cmd->player);
	const char *name = cmd->argv[1];
	dbref i;
	OBJ *thing;
	int total = 0;

	if (*name == '\0') {
		thing = OBJECT(player->location);
	} else if (!(thing = ematch_all(player, name))) {
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player->owner, REF(thing))) {
		notify(REF(player), "Permission denied. (You can't get the contents of something you don't control)");
		return;
	}
	DOLIST(i, thing->contents) {
		display_objinfo(REF(player), i, 0);
		total++;
	}
	notify(REF(player), "***End of List***");
	notifyf(REF(player), "%d objects found.", total);
}

static const char *look_c_version = "$RCSfile$ $Revision: 1.29 $";
const char *get_look_c_version(void) { return look_c_version; }
