#include "uapi/io.h"

#include "uapi/entity.h"
#include "uapi/match.h"

int ok_name(const char *name);

/* TODO improve/remove this. use skeletons to copy objects? */
void
do_clone(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	char *name = argv[1];
	int    cost;

	/* Perform sanity checks */

	if (!(ent_get(player_ref).flags & (EF_WIZARD | EF_BUILDER))) {
		nd_writef(player_ref, "That command is restricted to authorized builders.\n");
		return;
	}
	
	if (*name == '\0') {
		nd_writef(player_ref, "Clone what?\n");
		return;
	} 

	/* All OK so far, so try to find the thing that should be cloned. We
	   do not allow rooms, exits, etc. to be cloned for now. */

	if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
	   )
	{
		nd_writef(player_ref, "I don't know what you mean.\n");
		return;
	}

	/* Further sanity checks */
	OBJ thing;
	lhash_get(obj_hd, &thing, thing_ref);

	/* things only. */
	if(thing.type != TYPE_THING) {
		nd_writef(player_ref, "That is not a cloneable object.\n");
		return;
	}		
	
	/* check the name again, just in case reserved name patterns have
	   changed since the original object was created. */
	if (!ok_name(thing.name)) {
		nd_writef(player_ref, "You cannot clone something with such a weird name!\n");
		return;
	}

	/* no stealing stuff. */
	if(!controls(player_ref, thing_ref)) {
		nd_writef(player_ref, "Permission denied. (you can't clone this)\n");
		return;
	}

	/* there ain't no such lunch as a free thing. */
	cost = thing.value;
	
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	if (!payfor(player_ref, &player, cost)) {
		nd_writef(player_ref, "Sorry, you don't have enough shekels.\n");
		return;
	} else {
		/* create the object */
		lhash_put(obj_hd, player_ref, &player);
		OBJ clone;
		unsigned clone_ref = object_new(&clone);

		/* initialize everything */
		clone.name = strdup(thing.name);
		clone.location = player_ref;
		clone.owner = player.owner;
		clone.value = thing.value;
		/* FIXME: should we clone attached actions? */
		switch (thing.type) {
			case TYPE_ROOM:
				{
					ROO *rclone = &clone.sp.room;
					rclone->exits = rclone->doors = 0;
				}
				break;
			case TYPE_ENTITY:
				{
					ENT eclone = ent_get(clone_ref);
					eclone.home = ent_get(thing_ref).home;
					ent_set(clone_ref, &eclone);
				}
				break;
		}
		clone.type = thing.type;

		/* link it in */
		lhash_put(obj_hd, clone_ref, &clone);
		object_move(clone_ref, player_ref);

		/* and we're done */
		nd_writef(player_ref, "%s created with number %d.\n", thing.name, clone_ref);
	}
}

/*
 * do_create
 *
 * Use this to create an object.
 */
void
do_create(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	unsigned pflags = ent_get(player_ref).flags;
	char *name = argv[1];
	int cost = 30;

	if (!(pflags & (EF_WIZARD | EF_BUILDER))) {
		nd_writef(player_ref, "That command is restricted to authorized builders.\n");
		return;
	}
	if (*name == '\0') {
		nd_writef(player_ref, "Create what?\n");
		return;
	} else if (!ok_name(name)) {
		nd_writef(player_ref, "That's a silly name for a thing!\n");
		return;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);

	if (!payfor(player_ref, &player, cost)) {
		nd_writef(player_ref, "Sorry, you don't have enough shekels.\n");
		return;
	}

	lhash_put(obj_hd, player_ref, &player);

	OBJ thing;
	thing_ref = object_new(&thing);

	thing.name = strdup(name);
	thing.location = player_ref;
	thing.owner = player.owner;
	thing.value = cost;
	thing.type = TYPE_THING;

	lhash_put(obj_hd, thing_ref, &thing);
	object_move(thing_ref, player_ref);

	nd_writef(player_ref, "%s created with number %d.\n", name, thing_ref);
}
