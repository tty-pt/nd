#include "uapi/io.h"

#include "uapi/entity.h"
#include "uapi/match.h"

void
do_get(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref, cont_ref;
	char *what = argv[1];
	char *obj = argv[2];

	if (
			(thing_ref = ematch_near(player_ref, what)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, what)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	cont_ref = thing_ref;
	OBJ player = obj_get(player_ref);
	OBJ thing = obj_get(cont_ref), cont = thing;

	if (obj && *obj) {
		thing_ref = ematch_at(player_ref, cont_ref, obj);
		if (thing_ref == NOTHING) {
			nd_writef(player_ref, NOMATCH_MESSAGE);
			return;
		}
		thing = obj_get(thing_ref);
		if (cont.type == TYPE_ENTITY) {
			nd_writef(player_ref, "You can't steal from the living.\n");
			return;
		}
	}

	if (thing.location == player_ref) {
		nd_writef(player_ref, "You already have that!\n");
		return;
	}
	if (thing_ref == player_ref || thing_ref == player.location) {
		nd_writef(player_ref, "You can't pick yourself up by your bootstraps!\n");
		return;
	}
	switch (thing.type) {
	case TYPE_ENTITY:
	case TYPE_PLANT:
		if (player_ref == ROOT) {
			object_move(thing_ref, player_ref);
			nd_writef(player_ref, "Taken.\n");
		} else
			nd_writef(player_ref, "You can't pick that up.\n");
		break;
	case TYPE_SEAT:
		if (thing.owner != player_ref)
			nd_writef(player_ref, "You can't pick that up.\n");
		else {
			object_move(thing_ref, player_ref);
			nd_writef(player_ref, "Taken.\n");
		}
		break;
	case TYPE_ROOM:
		nd_writef(player_ref, "You can't take that!\n");
		break;
	default:
		object_move(thing_ref, player_ref);
		nd_writef(player_ref, "Taken.\n");
		break;
	}
}

void
do_drop(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref, cont_ref;
	OBJ player = obj_get(player_ref);
	char *name = argv[1];
	char *obj = argv[2];

	if ((thing_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	cont_ref = player.location;
	if (
			obj && *obj
			&& (cont_ref = ematch_mine(player_ref, obj)) == NOTHING
			&& (cont_ref = ematch_near(player_ref, obj)) == NOTHING
	   )
	{
		nd_writef(player_ref, "I don't know what you mean.\n");
		return;
	}
        
	if (thing_ref == cont_ref) {
		nd_writef(player_ref, "You can't put something inside of itself.\n");
		return;
	}

	OBJ cont = obj_get(cont_ref);

	if (cont.type != TYPE_ROOM && cont.type != TYPE_ENTITY &&
		!object_item(cont_ref)) {
		nd_writef(player_ref, "You can't put anything in that.\n");
		return;
	}

	object_move(thing_ref, cont_ref);
	OBJ thing = obj_get(thing_ref);

	if (object_item(cont_ref)) {
		nd_writef(player_ref, "Put away.\n");
		return;
	} else if (cont.type == TYPE_ENTITY) {
		nd_writef(cont_ref, "%s hands you %s.\n", player.name, thing.name);
		nd_writef(player_ref, "You hand %s to %s.\n", thing.name, cont.name);
		return;
	}

	nd_writef(player_ref, "Dropped.\n");
	nd_owritef(player_ref, "%s drops %s.\n", player.name, thing.name);
}

void
do_recycle(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	unsigned thing_ref;

	if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ thing = obj_get(thing_ref);

	if (!controls(player_ref, thing_ref)) {
		nd_writef(player_ref, "You can not do that.\n");
	} else {
		OBJ player = obj_get(player_ref);
		switch (thing.type) {
		case TYPE_ROOM:
			if (thing.owner != player.owner) {
				nd_writef(player_ref, "Permission denied.\n");
				return;
			}
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_SEAT:
			if (thing.owner != player.owner) {
				nd_writef(player_ref, "Permission denied.\n");
				return;
			}
			break;
		case TYPE_ENTITY:
			if (ent_get(thing_ref).flags & EF_PLAYER) {
				nd_writef(player_ref, "You can't recycle a player!\n");
				return;
			}
			break;
		}
		nd_writef(player_ref, "Thank you for recycling %.512s (#%d).\n", thing.name, thing_ref);
		object_move(thing_ref, NOTHING);
	}
}
