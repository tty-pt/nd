#include "io.h"

/* $Header$ */

#include "copyright.h"
#include "entity.h"
#include "config.h"
#include "params.h"

/* commands which look at things */

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "view.h"

/* prints owner of something */
static void
print_owner(OBJ *player, OBJ *thing)
{
	ENT *eplayer = &player->sp.entity;
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
	notify(eplayer, buf);
}

void
do_examine(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *thing, *content;
	char buf[BUFFER_LEN];

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		print_owner(player, thing);
		return;
	}
	switch (thing->type) {
	case TYPE_ROOM:
	case TYPE_PLANT:
	case TYPE_SEAT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_ENTITY:
		snprintf(buf, sizeof(buf), "%.*s (#%d) Owner: %s  Value: %d",
				(int) (BUFFER_LEN - strlen(thing->owner->name) - 35),
				unparse(player, thing),
				object_ref(thing),
				thing->owner->name, thing->value);
		break;
	case TYPE_GARBAGE:
		strlcpy(buf, unparse(player, thing), sizeof(buf));
		break;
	}
	notify(eplayer, buf);

	if (thing->description)
		notify(eplayer, thing->description);

	notify(eplayer, "[ Use 'examine <object>=/' to list root properties. ]");

	/* show him the contents */
	if (thing->contents) {
		notify(eplayer, "Contents:");
		FOR_LIST(content, thing->contents) {
			notify(eplayer, unparse(player, content));
		}
	}
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			notifyf(eplayer, "Exits: %hhx Doors: %hhx", rthing->exits, rthing->doors);

			if (rthing->dropto) {
				snprintf(buf, sizeof(buf), "Dropped objects go to: %s",
						unparse(player, rthing->dropto));
				notify(eplayer, buf);
			}
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *ething = &thing->sp.equipment;
			notifyf(eplayer, "equipment eqw %u msv %u.", ething->eqw, ething->msv);
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pthing = &thing->sp.plant;
			notifyf(eplayer, "plant plid %u size %u.", pthing->plid, pthing->size);
		}
		break;
	case TYPE_SEAT:
		{
			SEA *sthing = &thing->sp.seat;
			notifyf(eplayer, "seat quantity %u capacity %u.", sthing->quantity, sthing->capacity);
		}
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (thing->location && controls(player, thing->location)) {
			snprintf(buf, sizeof(buf), "Location: %s", unparse(player, thing->location));
			notify(eplayer, buf);
		}
		break;
	case TYPE_ENTITY:
		{
			ENT *ething = &thing->sp.entity;

			snprintf(buf, sizeof(buf), "Home: %s", unparse(player, ething->home));
			notify(eplayer, buf);
			notifyf(eplayer, "hp: %d/%d entity flags: %d", ething->hp, HP_MAX(ething), ething->flags);

			/* print location if player can link to it */
			if (thing->location && controls(player, thing->location)) {
				snprintf(buf, sizeof(buf), "Location: %s", unparse(player, thing->location));
				notify(eplayer, buf);
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
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	char buf[BUFFER_LEN];

	snprintf(buf, sizeof(buf), "You have %d %s.", player->value,
			player->value == 1 ? PENNY : PENNIES);
	notify(eplayer, buf);
}

void
do_inventory(command_t *cmd)
{
	OBJ *player = cmd->player,
	    *thing;
	ENT *eplayer = &player->sp.entity;

        if (mcp_look(player, player)) {
		thing = player->contents;
                if (!thing) {
                        notify(eplayer, "You aren't carrying anything.");
                } else {
                        notify(eplayer, "You are carrying:");
                        FOR_LIST(thing, thing) {
                                notify(eplayer, unparse(player, thing));
                        }
                }
        }

	do_score(cmd);
}

static void
display_objinfo(OBJ *player, OBJ *obj, int output_type)
{
	ENT *eplayer = &player->sp.entity;
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];

	strlcpy(buf2, unparse(player, obj), sizeof(buf2));

	switch (output_type) {
	case 1:					/* owners */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse(player, obj->owner));
		break;
	case 2:					/* links */
		switch (obj->type) {
		case TYPE_ROOM:
			{
				ROO *robj = &obj->sp.room;
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
						unparse(player, robj->dropto));
			}

			break;
		case TYPE_ENTITY:
			{
				ENT *eobj = &obj->sp.entity;
				snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, unparse(player, eobj->home));
			}

			break;
		default:
			snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2, "N/A");
			break;
		}
		break;
	case 3:					/* locations */
		snprintf(buf, sizeof(buf), "%-38.512s  %.512s", buf2,
				unparse(player, obj->location));
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
	notify(eplayer, buf);
}


void
do_find(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	dbref i;
	char buf[BUFFER_LEN + 2];
	int total = 0;

	strlcpy(buf, "*", sizeof(buf));
	strlcat(buf, name, sizeof(buf));
	strlcat(buf, "*", sizeof(buf));

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(eplayer, "You don't have enough %s.", PENNIES);
	} else {
		for (i = 0; i < db_top; i++) {
			OBJ *oi = object_get(i);
			if (((eplayer->flags & EF_WIZARD) || oi->owner == player->owner) &&
				oi->name && (!*name || !strcmp(buf, (char *) oi->name))) {
				display_objinfo(player, oi, 0);
				total++;
			}
		}
		notify(eplayer, "***End of List***");
		notifyf(eplayer, "%d objects found.", total);
	}
}


void
do_owned(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *victim;
	dbref i;
	int total = 0;

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(eplayer, "You don't have enough %s.", PENNIES);
		return;
	}
	if ((eplayer->flags & EF_WIZARD) && *name) {
		victim = lookup_player(name);
		if (!victim) {
			notify(eplayer, "I couldn't find that player.");
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
	notify(eplayer, "***End of List***");
	notifyf(eplayer, "%d objects found.", total);
}

void
do_contents(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *oi, *thing;
	int total = 0;

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		notify(eplayer, "Permission denied. (You can't get the contents of something you don't control)");
		return;
	}
	FOR_LIST(oi, thing->contents) {
		display_objinfo(player, oi, 0);
		total++;
	}
	notify(eplayer, "***End of List***");
	notifyf(eplayer, "%d objects found.", total);
}
