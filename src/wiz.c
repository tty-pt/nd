#include "uapi/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "config.h"
#include "uapi/entity.h"
#include "uapi/match.h"
#include "player.h"

void
do_teleport(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), victim_ref, destination_ref;
	char *arg1 = argv[1];
	char *arg2 = argv[2];
	char *to;

	/* get victim, destination */
	if (*arg2 == '\0') {
		victim_ref = player_ref;
		to = arg1;
	} else if ((victim_ref = ematch_all(player_ref, arg1)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

	OBJ victim;
	qdb_get(obj_hd, &victim, &victim_ref);

	destination_ref = ematch_all(player_ref, to);

	if (destination_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ destination;
	qdb_get(obj_hd, &destination, &destination_ref);

	if (victim_ref == destination_ref || destination.location == victim_ref)
		goto error;

	switch (victim.type) {
	case TYPE_ENTITY:
		if (!controls(player_ref, victim_ref) ||
				!controls(player_ref, destination_ref) ||
				!controls(player_ref, victim.location) ||
				(object_item(destination_ref) && !controls(player_ref, destination.location)) ||
				destination.type != TYPE_ROOM)
			goto error;
		nd_writef(victim_ref, "You feel a wrenching sensation...\n");
		enter(victim_ref, destination_ref, E_NULL);
		return;
	case TYPE_ROOM:
		goto error;
	default:
		if ((!object_item(destination_ref)) || !(
			controls(player_ref, destination_ref) &&
			(controls(player_ref, victim_ref) || controls(player_ref, victim.location)) )
		   )
			goto error;
		object_move(victim_ref, destination_ref);
		return;
	}
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

void
do_ban(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), victim_ref;
	char *name = argv[1];

	if (!(ent_get(player_ref).flags & EF_WIZARD))
		goto error;

	victim_ref = player_get(name);

	if (victim_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	if (victim_ref == ROOT)
		goto error;

	OBJ victim;
	qdb_get(obj_hd, &victim, &victim_ref);

	if (victim.type != TYPE_ENTITY)
		goto error;

	ENT evictim = ent_get(victim_ref);
	evictim.flags |= EF_BAN;
	ent_set(player_ref, &evictim);
	nd_writef(victim_ref, "You have been banned.\n");
	nd_close(victim_ref);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}
