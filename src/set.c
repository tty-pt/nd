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
	ENT *eplayer = &player->sp.entity;
	OBJ *match = ematch_all(player, name);

	if (!match) {
		ndc_writef(eplayer->fd, NOMATCH_MESSAGE);
		return NULL;
	}

	else if (!controls(player, match)) {
		ndc_writef(eplayer->fd, "Permission denied. (You don't control what was matched)\n");
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
		ndc_writef(fd, "Give it what new name?\n");
		return;
	}
	/* check for renaming a player */
	if (!ok_name(newname)) {
		ndc_writef(fd, "That is not a reasonable name.\n");
		return;
	}

	/* everything ok, change the name */
	if (thing->name) {
		free((void *) thing->name);
	}
	thing->name = strdup(newname);
	ndc_writef(fd, "Name set.\n");
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
		ndc_writef(fd, "You must specify what you want to take ownership of.\n");
		return;
	}

	thing = ematch_all(player, name);
	if (!thing) {
		ndc_writef(fd, NOMATCH_MESSAGE);
		return;
	}

	if (*newowner && strcmp(newowner, "me")) {
		owner = player_get(newowner);

		if (!owner) {
			ndc_writef(fd, "I couldn't find that player.\n");
			return;
		}
	} else {
		owner = player->owner;
	}
	if (!(eplayer->flags & EF_WIZARD) && player->owner != owner) {
		ndc_writef(fd, "Only wizards can transfer ownership to others.\n");
		return;
	}

	if ((eplayer->flags & EF_WIZARD) && !God(player) && God(owner)) {
		ndc_writef(fd, "God doesn't need an offering or sacrifice.\n");
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		if (!(eplayer->flags & EF_WIZARD) && player->location != thing) {
			ndc_writef(fd, "You can only chown \"here\".\n");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(eplayer->flags & EF_WIZARD) && thing->location != player) {
			ndc_writef(fd, "You aren't carrying that.\n");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_ENTITY:
		ndc_writef(fd, "Entities always own themselves.\n");
		return;
	case TYPE_GARBAGE:
		ndc_writef(fd, "No one wants to own garbage.\n");
		return;
	}
	if (owner == player)
		ndc_writef(fd, "Owner changed to you.\n");
	else {
		ndc_writef(fd, "Owner changed to %s.\n", unparse(player, owner));
	}
}
