#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <ndc.h>

#ifndef __OpenBSD__
#include <bsd/string.h>
#endif

#include "io.h"
#include "entity.h"
#include "config.h"
#include "utils.h"

#include "params.h"
#include "defaults.h"
#include "match.h"

/* TODO improve/remove this. use skeletons to copy objects? */
void
do_clone(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd), thing_ref;
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
	OBJ thing = obj_get(thing_ref);

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
	cost = OBJECT_GETCOST(thing.value);
	if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	
	OBJ player = obj_get(player_ref);
	if (!payfor(player_ref, &player, cost)) {
		nd_writef(player_ref, "Sorry, you don't have enough %s.\n", PENNIES);
		return;
	} else {
		/* create the object */
		obj_set(player_ref, &player);
		OBJ clone;
		dbref clone_ref = object_new(&clone);

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
				ENT_SETF(clone_ref, home, ent_get(thing_ref).home);
				break;
		}
		clone.type = thing.type;

		/* link it in */
		obj_set(clone_ref, &clone);
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
	dbref player_ref = FD_PLAYER(fd), thing_ref;
	unsigned pflags = ent_get(player_ref).flags;
	char *name = argv[1];
	char *acost =argv[2];
	int cost;

	char buf2[BUFSIZ];
	char *rname, *qname;

	strlcpy(buf2, acost, sizeof(buf2));
	for (rname = buf2; (*rname && (*rname != '=')); rname++) ;
	qname = rname;
	if (*rname)
		*(rname++) = '\0';
	while ((qname > buf2) && (isspace(*qname)))
		*(qname--) = '\0';
	name = buf2;
	for (; *rname && isspace(*rname); rname++) ;

	cost = atoi(qname);
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
	} else if (cost < 0) {
		nd_writef(player_ref, "You can't create an object for less than nothing!\n");
		return;
	} else if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}

	OBJ player = obj_get(player_ref);

	if (!payfor(player_ref, &player, cost)) {
		nd_writef(player_ref, "Sorry, you don't have enough %s.\n", PENNIES);
		return;
	}

	obj_set(player_ref, &player);

	OBJ thing;
	thing_ref = object_new(&thing);

	thing.name = strdup(name);
	thing.location = player_ref;
	thing.owner = player.owner;
	thing.value = OBJECT_ENDOWMENT(cost);
	thing.type = TYPE_THING;

	obj_set(thing_ref, &thing);
	object_move(thing_ref, player_ref);

	nd_writef(player_ref, "%s created with number %d.\n", name, thing_ref);
}
