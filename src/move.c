#include "io.h"
#include "entity.h"
#include "config.h"

#include "params.h"
#include "defaults.h"
#include "match.h"
#include <ndc.h>

void
do_get(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *what = argv[1];
	char *obj = argv[2];
	OBJ *thing, *cont;

	if (
			!(thing = ematch_near(player, what))
			&& !(thing = ematch_mine(player, what))
	   )
	{
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	cont = thing;

	// is this needed?
	if (thing->location != player->location && !(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "That is too far away from you.\n");
		return;
	}

	if (obj && *obj) {
		thing = ematch_at(player, cont, obj);
		if (!thing)
			return;
		if (cont->type == TYPE_ENTITY) {
			nd_writef(player, "You can't steal from the living.\n");
			return;
		}
	}
	if (thing->location == player) {
		nd_writef(player, "You already have that!\n");
		return;
	}
	if (object_plc(thing, player)) {
		nd_writef(player, "You can't pick yourself up by your bootstraps!\n");
		return;
	}
	switch (thing->type) {
	case TYPE_ENTITY:
	case TYPE_PLANT:
		if (God(player)) {
			object_move(thing, player);
			nd_writef(player, "Taken.\n");
		} else
			nd_writef(player, "You can't pick that up.\n");
		break;
	case TYPE_SEAT:
		if (thing->owner != player)
			nd_writef(player, "You can't pick that up.\n");
		else {
			object_move(thing, player);
			nd_writef(player, "Taken.\n");
		}
		break;
	case TYPE_ROOM:
		nd_writef(player, "You can't take that!\n");
		break;
	default:
		object_move(thing, player);
		nd_writef(player, "Taken.\n");
		break;
	}
}

void
do_drop(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	char *obj = argv[2];
	OBJ *loc = player->location,
	    *thing, *cont;

	if (!(thing = ematch_mine(player, name))) {
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	cont = loc;
	if (
			obj && *obj
			&& !(cont = ematch_mine(player, obj))
			&& !(cont = ematch_near(player, obj))
	   )
	{
		nd_writef(player, "I don't know what you mean.\n");
		return;
	}
        
	if (thing->type == TYPE_GARBAGE) {
		nd_writef(player, "You can't drop that.\n");
		return;
	}

	if (cont->type != TYPE_ROOM && cont->type != TYPE_ENTITY &&
		!object_item(cont)) {
		nd_writef(player, "You can't put anything in that.\n");
		return;
	}
	if (object_plc(thing, cont)) {
		nd_writef(player, "You can't put something inside of itself.\n");
		return;
	}

	object_move(thing, cont);

	if (object_item(cont)) {
		nd_writef(player, "Put away.\n");
		return;
	} else if (cont->type == TYPE_ENTITY) {
		nd_writef(cont, "%s hands you %s.\n", player->name, thing->name);
		nd_writef(player, "You hand %s to %s.\n", thing->name, cont->name);
		return;
	}

	nd_writef(player, "Dropped.\n");
	nd_owritef(player, "%s drops %s.\n", player->name, thing->name);
}

void
do_recycle(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	OBJ *thing;

	if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
	   )
	{
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}


	if(!God(player) && God(thing->owner)) {
		nd_writef(player, "Only God may reclaim God's property.\n");
		return;
	}

	if (!controls(player, thing)) {
		nd_writef(player, "You can not do that.\n");
	} else {
		switch (thing->type) {
		case TYPE_ROOM:
			if (thing->owner != player->owner) {
				nd_writef(player, "Permission denied. (You don't control the room you want to recycle)\n");
				return;
			}
			if (thing == PLAYER_START) {
				nd_writef(player, "That is the player start room, and may not be recycled.\n");
				return;
			}
			if (thing == GLOBAL_ENVIRONMENT) {
				nd_writef(player, "If you want to do that, why don't you just delete the database instead?  Room #0 contains everything, and is needed for database sanity.\n");
				return;
			}
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_SEAT:
			if (thing->owner != player->owner) {
				nd_writef(player, "Permission denied. (You can't recycle a thing you don't control)\n");
				return;
			}
			break;
		case TYPE_ENTITY:
			if (thing->sp.entity.flags & EF_PLAYER) {
				nd_writef(player, "You can't recycle a player!\n");
				return;
			}
			break;
		case TYPE_GARBAGE:
			nd_writef(player, "That's already garbage!\n");
			return;
		}
		nd_writef(player, "Thank you for recycling %.512s (#%d).\n", thing->name, thing);
		recycle(player, thing);
	}
}
