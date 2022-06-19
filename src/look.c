#include "io.h"

#include "entity.h"

#include "defaults.h"
#include "match.h"
#include "player.h"
#include "command.h"

/* prints owner of something */
static inline void
print_owner(ENT *eplayer, OBJ *thing)
{
	switch (thing->type) {
	case TYPE_ENTITY:
		notifyf(eplayer, "%s is an entity.", thing->name);
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		notifyf(eplayer, "Owner: %s", thing->owner->name);
		break;
	case TYPE_GARBAGE:
		notifyf(eplayer, "%s is garbage.", thing->name);
		break;
	}
}

void
do_examine(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *thing, *content;

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		print_owner(eplayer, thing);
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
		notifyf(eplayer, "%s (#%d) Owner: %s  Value: %d",
				unparse(player, thing),
				object_ref(thing),
				thing->owner->name, thing->value);
		break;
	case TYPE_GARBAGE:
		notifyf(eplayer, "%s", unparse(player, thing));
		break;
	}

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

			if (rthing->dropto)
				notifyf(eplayer, "Dropped objects go to: %s",
						unparse(player, rthing->dropto));
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
		if (thing->location && controls(player, thing->location))
			notifyf(eplayer, "Location: %s", unparse(player, thing->location));
		break;
	case TYPE_ENTITY:
		{
			ENT *ething = &thing->sp.entity;

			notifyf(eplayer, "Home: %s", unparse(player, ething->home));
			notifyf(eplayer, "hp: %d/%d entity flags: %d", ething->hp, HP_MAX(ething), ething->flags);

			/* print location if player can link to it */
			if (thing->location && controls(player, thing->location))
				notifyf(eplayer, "Location: %s", unparse(player, thing->location));
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

	notifyf(eplayer, "You have %d %s.", player->value,
			player->value == 1 ? PENNY : PENNIES);
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
display_objinfo(OBJ *player, OBJ *obj)
{
	ENT *eplayer = &player->sp.entity;
	notifyf(eplayer, "%s", unparse(player, obj));
}

void
do_owned(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *victim, *oi;
	int total = 0;

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(eplayer, "You don't have enough %s.", PENNIES);
		return;
	}
	if ((eplayer->flags & EF_WIZARD) && *name) {
		victim = player_get(name);
		if (!victim) {
			notify(eplayer, "I couldn't find that player.");
			return;
		}
	} else
		victim = player;

	FOR_ALL(oi) {
		if (oi->owner == victim->owner) {
			display_objinfo(player, oi);
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
		display_objinfo(player, oi);
		total++;
	}
	notify(eplayer, "***End of List***");
	notifyf(eplayer, "%d objects found.", total);
}
