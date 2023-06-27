#include "io.h"
#include "entity.h"
#include "config.h"
#include "command.h"

#include "params.h"
#include "defaults.h"
#include "match.h"

void
do_get(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *obj = cmd->argv[2];
	OBJ *thing, *cont;

	if (
			!(thing = ematch_near(player, what))
			&& !(thing = ematch_mine(player, what))
	   )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	cont = thing;

	// is this needed?
	if (thing->location != player->location && !(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "That is too far away from you.");
		return;
	}

	if (obj && *obj) {
		thing = ematch_at(player, cont, obj);
		if (!thing)
			return;
		if (cont->type == TYPE_ENTITY) {
			notify(eplayer, "You can't steal from the living.");
			return;
		}
	}
	if (thing->location == player) {
		notify(eplayer, "You already have that!");
		return;
	}
	if (object_plc(thing, player)) {
		notify(eplayer, "You can't pick yourself up by your bootstraps!");
		return;
	}
	switch (thing->type) {
	case TYPE_ENTITY:
	case TYPE_PLANT:
		if (God(player)) {
			object_move(thing, player);
			notify(eplayer, "Taken.");
		} else
			notify(eplayer, "You can't pick that up.");
		break;
	case TYPE_SEAT:
		if (thing->owner != player)
			notify(eplayer, "You can't pick that up.");
		else {
			object_move(thing, player);
			notify(eplayer, "Taken.");
		}
		break;
	case TYPE_ROOM:
		notify(eplayer, "You can't take that!");
		break;
	default:
		object_move(thing, player);
		notify(eplayer, "Taken.");
		break;
	}
}

void
do_drop(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	const char *obj = cmd->argv[2];
	OBJ *loc = player->location,
	    *thing, *cont;

	if (!(thing = ematch_mine(player, name))) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	cont = loc;
	if (
			obj && *obj
			&& !(cont = ematch_mine(player, obj))
			&& !(cont = ematch_near(player, obj))
	   )
	{
		notify(eplayer, "I don't know what you mean.");
		return;
	}
        
	switch (thing->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_ENTITY:
	case TYPE_THING:
		if (cont->type != TYPE_ROOM && cont->type != TYPE_ENTITY &&
			!object_item(cont)) {
			notify(eplayer, "You can't put anything in that.");
			break;
		}
		if (object_plc(thing, cont)) {
			notify(eplayer, "You can't put something inside of itself.");
			break;
		}

		int immediate_dropto = (cont->type == TYPE_ROOM && cont->sp.room.dropto);

		object_move(thing, immediate_dropto ? cont->sp.room.dropto : cont);

		if (object_item(cont)) {
			notify(eplayer, "Put away.");
			return;
		} else if (cont->type == TYPE_ENTITY) {
			ENT *econt = &cont->sp.entity;
			notifyf(econt, "%s hands you %s", player->name, thing->name);
			notifyf(eplayer, "You hand %s to %s", thing->name, cont->name);
			return;
		}

		notify(eplayer, "Dropped.");
		onotifyf(player, "%s drops %s.", player->name, thing->name);
		break;
	default:
		notify(eplayer, "You can't drop that.");
		break;
	}
}

void
do_recycle(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
	   )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}


	if(!God(player) && God(thing->owner)) {
		notify(eplayer, "Only God may reclaim God's property.");
		return;
	}

	if (!controls(player, thing)) {
		notify(eplayer, "You can not do that.");
	} else {
		switch (thing->type) {
		case TYPE_ROOM:
			if (thing->owner != player->owner) {
				notify(eplayer, "Permission denied. (You don't control the room you want to recycle)");
				return;
			}
			if (thing == PLAYER_START) {
				notify(eplayer, "That is the player start room, and may not be recycled.");
				return;
			}
			if (thing == GLOBAL_ENVIRONMENT) {
				notify(eplayer, "If you want to do that, why don't you just delete the database instead?  Room #0 contains everything, and is needed for database sanity.");
				return;
			}
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_SEAT:
			if (thing->owner != player->owner) {
				notify(eplayer, "Permission denied. (You can't recycle a thing you don't control)");
				return;
			}
			break;
		case TYPE_ENTITY:
			if (thing->sp.entity.flags & EF_PLAYER) {
				notify(eplayer, "You can't recycle a player!");
				return;
			}
			break;
		case TYPE_GARBAGE:
			notify(eplayer, "That's already garbage!");
			return;
			/* NOTREACHED */
			break;
		}
		notifyf(eplayer, "Thank you for recycling %.512s (#%d).", thing->name, thing);
		recycle(player, thing);
	}
}
