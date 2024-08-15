#include "uapi/io.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ndc.h>

#include "params.h"
#include "player.h"
#include "uapi/entity.h"
#include "uapi/match.h"

static unsigned
match_controlled(unsigned player_ref, char *name)
{
	unsigned match_ref = ematch_all(player_ref, name);

	if (match_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return NOTHING;
	}

	else if (!controls(player_ref, match_ref)) {
		nd_writef(player_ref, "Permission denied. (You don't control what was matched)\n");
		return NOTHING;
	}

	return match_ref;
}

int ok_name(const char *name);

void
do_name(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	char *newname = argv[2];
	unsigned thing_ref = match_controlled(player_ref, name);

	if (thing_ref == NOTHING)
		return;

	/* check for bad name */
	if (*newname == '\0') {
		nd_writef(player_ref, "Give it what new name?\n");
		return;
	}
	/* check for renaming a player */
	if (!ok_name(newname)) {
		nd_writef(player_ref, "That is not a reasonable name.\n");
		return;
	}

	/* everything ok, change the name */
	OBJ thing = obj_get(thing_ref);
	if (thing.name)
		free((void *) thing.name);
	thing.name = strdup(newname);
	obj_set(thing_ref, &thing);
	nd_writef(player_ref, "Name set.\n");
}

void
do_chown(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), owner_ref, thing_ref;
	int wizard = ent_get(player_ref).flags & EF_WIZARD;
	char *name = argv[1];
	char *newowner = argv[2];

	if (!*name) {
		nd_writef(player_ref, "You must specify what you want to take ownership of.\n");
		return;
	}

	thing_ref = ematch_all(player_ref, name);
	if (thing_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ player = obj_get(player_ref);

	if (*newowner && strcmp(newowner, "me")) {
		owner_ref = player_get(newowner);

		if (owner_ref == NOTHING) {
			nd_writef(player_ref, "I couldn't find that player.\n");
			return;
		}
	} else
		owner_ref = player.owner;
	if (!wizard && player.owner != owner_ref) {
		nd_writef(player_ref, "Only wizards can transfer ownership to others.\n");
		return;
	}

	OBJ owner = obj_get(owner_ref);
	OBJ thing = obj_get(thing_ref);

	switch (thing.type) {
	case TYPE_ROOM:
		if (!wizard && player.location != thing_ref) {
			nd_writef(player_ref, "You can only chown \"here\".\n");
			return;
		}
		thing.owner = owner.owner;
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!wizard && thing.location != player_ref) {
			nd_writef(player_ref, "You aren't carrying that.\n");
			return;
		}
		thing.owner = owner.owner;
		break;
	case TYPE_ENTITY:
		nd_writef(player_ref, "Entities always own themselves.\n");
		return;
	}
	if (owner_ref == player_ref)
		nd_writef(player_ref, "Owner changed to you.\n");
	else {
		nd_writef(player_ref, "Owner changed to %s.\n", unparse(owner_ref));
	}

	obj_set(thing_ref, &thing);
}
