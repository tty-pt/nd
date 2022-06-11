
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
print_owner(OBJ *player, OBJ *thing)
{
	char buf[BUFFER_LEN];

	switch (thing->type) {
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%s is an entity.", thing->name);
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		snprintf(buf, sizeof(buf), "Owner: %s", thing->owner->name);
		break;
	case TYPE_GARBAGE:
		snprintf(buf, sizeof(buf), "%s is garbage.", thing->name);
		break;
	}
	notify(player, buf);
}

static void
look_contents(OBJ *player, OBJ *loc, const char *contents_name)
{
        char buf[BUFSIZ];
        size_t buf_l = 0;
	OBJ *thing;

	/* check to see if there is anything there */
	if (loc->contents) {
                DOLIST(thing, loc->contents) {
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
look_simple(OBJ *player, OBJ *thing)
{
	char const *art_str = thing->art;
	ENT *eplayer = &player->sp.entity;

	if (art_str)
		art(eplayer->fd, art_str);

	if (thing->description) {
		notify(player, thing->description);
	} else if (!art_str) {
		notify(player, "You see nothing special.");
	}
}

void
look_room(OBJ *player, OBJ *loc)
{
	char const *description = "";
	CBUG(loc->type != TYPE_ROOM);
	can_doit(player, loc, 0);

	if (web_look(player, loc)) {
		notify(player, unparse_object(player, loc));
		notify(player, description);
		look_contents(player, loc, "You see:");
	}
}

void
look_around(OBJ *player)
{
	look_room(player, player->location);
}

void
do_look_at(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (*name == '\0') {
		thing = player->location;
	} else if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_here(player, name))
			&& !(thing = ematch_me(player, name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
		  )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		look_room(player, thing);
		view(player);
		break;
	case TYPE_ENTITY:
		if (web_look(player, thing)) {
			look_simple(player, thing);
			look_contents(player, thing, "Carrying:");
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (web_look(player, thing))
			look_simple(player, thing);
		break;
	default:
		if (web_look(player, thing))
			look_simple(player, thing);
		break;
	}
}

int
listprops_wildcard(OBJ *player, OBJ *thing, const char *dir, const char *wild)
{
	ENT *eplayer = &player->sp.entity;
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
				|| eplayer->flags & EF_WIZARD)) {
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
size_object(OBJ *obj, int load)
{
	long byts;
	byts = sizeof(struct object);

	if (obj->name) {
		byts += strlen(obj->name) + 1;
	}
	byts += size_properties(obj, load);

	return byts;
}


void
do_examine(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	const char *dir = cmd->argv[2];
	OBJ *thing, *content;
	char buf[BUFFER_LEN];
	int cnt;

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
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
	switch (thing->type) {
	case TYPE_ROOM:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Parent: ",
				(int) (BUFFER_LEN - strlen(thing->owner->name) - 35),
				unparse_object(player, thing),
				object_ref(thing),
				thing->owner->name);
		strlcat(buf, unparse_object(player, thing->location), sizeof(buf));
		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Value: %d",
				(int) (BUFFER_LEN - strlen(thing->owner->name) - 35),
				unparse_object(player, thing),
				object_ref(thing),
				thing->owner->name, thing->value);
		break;
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%.*s (#%d) %s: %d  ", 
				(int) (BUFFER_LEN - strlen(CPENNIES) - 35),
				unparse_object(player, thing),
				object_ref(thing),
				CPENNIES, thing->value);
		break;
	case TYPE_GARBAGE:
		strlcpy(buf, unparse_object(player, thing), sizeof(buf));
		break;
	}
	notify(player, buf);

	if (thing->description)
		notify(player, thing->description);

	notify(player, "[ Use 'examine <object>=/' to list root properties. ]");

	snprintf(buf, sizeof(buf), "Memory used: %ld bytes", size_object(thing, 1));
	notify(player, buf);

	/* show him the contents */
	if (thing->contents) {
		notify(player, "Contents:");
		DOLIST(content, thing->contents) {
			notify(player, unparse_object(player, content));
		}
	}
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			notifyf(player, "Exits: %hhx Doors: %hhx", rthing->exits, rthing->doors);

			if (rthing->dropto) {
				snprintf(buf, sizeof(buf), "Dropped objects go to: %s",
						unparse_object(player, rthing->dropto));
				notify(player, buf);
			}
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *ething = &thing->sp.equipment;
			notifyf(player, "equipment eqw %u msv %u.", ething->eqw, ething->msv);
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pthing = &thing->sp.plant;
			notifyf(player, "plant plid %u size %u.", pthing->plid, pthing->size);
		}
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (thing->location && controls(player, thing->location)) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse_object(player, thing->location));
			notify(player, buf);
		}
		break;
	case TYPE_ENTITY:
		{
			ENT *ething = &thing->sp.entity;

			snprintf(buf, sizeof(buf), "Home: %s", unparse_object(player, ething->home));
			notify(player, buf);
			notifyf(player, "hp: %d/%d entity flags: %d", ething->hp, HP_MAX(ething), ething->flags);

			/* print location if player can link to it */
			if (thing->location && controls(player, thing->location)) {
				snprintf(buf, sizeof(buf), "Location: %s", unparse_object(player, thing->location));
				notify(player, buf);
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
	OBJ *player = object_get(cmd->player);
	char buf[BUFFER_LEN];

	snprintf(buf, sizeof(buf), "You have %d %s.", player->value,
			player->value == 1 ? PENNY : PENNIES);
	notify(player, buf);
}

void
do_inventory(command_t *cmd)
{
	OBJ *player = object_get(cmd->player),
	    *thing;

        if (web_look(player, player)) {
		thing = player->contents;
                if (!thing) {
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
display_objinfo(OBJ *player, OBJ *obj, int output_type)
{
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];

	strlcpy(buf2, unparse_object(player, obj), sizeof(buf2));

	switch (output_type) {
	case 1:					/* owners */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, obj->owner));
		break;
	case 2:					/* links */
		switch (obj->type) {
		case TYPE_ROOM:
			{
				ROO *robj = &obj->sp.room;
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
						unparse_object(player, robj->dropto));
			}

			break;
		case TYPE_ENTITY:
			{
				ENT *eobj = &obj->sp.entity;
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse_object(player, eobj->home));
			}

			break;
		default:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "N/A");
			break;
		}
		break;
	case 3:					/* locations */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
				unparse_object(player, obj->location));
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
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
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
			OBJ *oi = object_get(i);
			if (((eplayer->flags & EF_WIZARD) || oi->owner == player->owner) &&
				oi->name && (!*name || equalstr(buf, (char *) oi->name))) {
				display_objinfo(player, oi, 0);
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
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *victim;
	dbref i;
	int total = 0;

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(player, "You don't have enough %s.", PENNIES);
		return;
	}
	if ((eplayer->flags & EF_WIZARD) && *name) {
		victim = lookup_player(name);
		if (!victim) {
			notify(player, "I couldn't find that player.");
			return;
		}
	} else
		victim = player;

	for (i = 0; i < db_top; i++) {
		OBJ *oi = object_get(i);
		if (oi->owner == victim->owner) {
			display_objinfo(player, oi, 0);
			total++;
		}
	}
	notify(player, "***End of List***");
	notifyf(player, "%d objects found.", total);
}

void
do_contents(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	OBJ *oi, *thing;
	int total = 0;

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		notify(player, "Permission denied. (You can't get the contents of something you don't control)");
		return;
	}
	DOLIST(oi, thing->contents) {
		display_objinfo(player, oi, 0);
		total++;
	}
	notify(player, "***End of List***");
	notifyf(player, "%d objects found.", total);
}

static const char *look_c_version = "$RCSfile$ $Revision: 1.29 $";
const char *get_look_c_version(void) { return look_c_version; }
