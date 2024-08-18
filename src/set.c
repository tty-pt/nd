#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ndc.h>

#include "io.h"
#include "copyright.h"
#include "entity.h"
#include "utils.h"
#include "params.h"
#include "defaults.h"
#include "match.h"
#include "player.h"

static OBJ *
match_controlled(OBJ *player, char *name)
{
	OBJ *match = ematch_all(player, name);

	if (!match) {
		nd_writef(player, NOMATCH_MESSAGE);
		return NULL;
	}

	else if (!controls(player, match)) {
		nd_writef(player, "Permission denied. (You don't control what was matched)\n");
		return NULL;
	}

	return match;
}

void
do_name(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	char *newname = argv[2];
	OBJ *thing = match_controlled(player, name);

	if (!thing)
		return;

	/* check for bad name */
	if (*newname == '\0') {
		nd_writef(player, "Give it what new name?\n");
		return;
	}
	/* check for renaming a player */
	if (!ok_name(newname)) {
		nd_writef(player, "That is not a reasonable name.\n");
		return;
	}

	/* everything ok, change the name */
	if (thing->name) {
		free((void *) thing->name);
	}
	thing->name = strdup(newname);
	nd_writef(player, "Name set.\n");
}

void
do_chown(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	char *newowner = argv[2];
	OBJ *thing;
	OBJ *owner;

	if (!*name) {
		nd_writef(player, "You must specify what you want to take ownership of.\n");
		return;
	}

	thing = ematch_all(player, name);
	if (!thing) {
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	if (*newowner && strcmp(newowner, "me")) {
		owner = player_get(newowner);

		if (!owner) {
			nd_writef(player, "I couldn't find that player.\n");
			return;
		}
	} else {
		owner = object_get(player->owner);
	}
	if (!(eplayer->flags & EF_WIZARD) && player->owner != object_ref(owner)) {
		nd_writef(player, "Only wizards can transfer ownership to others.\n");
		return;
	}

	if ((eplayer->flags & EF_WIZARD) && !God(player) && God(owner)) {
		nd_writef(player, "God doesn't need an offering or sacrifice.\n");
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		if (!(eplayer->flags & EF_WIZARD) && player->location != object_ref(thing)) {
			nd_writef(player, "You can only chown \"here\".\n");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(eplayer->flags & EF_WIZARD) && thing->location != object_ref(player)) {
			nd_writef(player, "You aren't carrying that.\n");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_ENTITY:
		nd_writef(player, "Entities always own themselves.\n");
		return;
	case TYPE_GARBAGE:
		nd_writef(player, "No one wants to own garbage.\n");
		return;
	}
	if (owner == player)
		nd_writef(player, "Owner changed to you.\n");
	else {
		nd_writef(player, "Owner changed to %s.\n", unparse(player, owner));
	}
}
