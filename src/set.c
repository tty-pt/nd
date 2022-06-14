/* $Header$ */
#include "io.h"


#include "copyright.h"
#include "entity.h"

/* commands which set parameters */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "match.h"
#include "interface.h"
#include "externs.h"

static OBJ *
match_controlled(OBJ *player, const char *name)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *match = ematch_all(player, name);

	if (!match) {
		notify(eplayer, NOMATCH_MESSAGE);
		return NULL;
	}

	else if (!controls(player, match)) {
		notify(eplayer, "Permission denied. (You don't control what was matched)");
		return NULL;
	}

	return match;
}

void
do_name(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	char *newname = cmd->argv[2];
	OBJ *thing = match_controlled(player, name);

	if (!thing)
		return;
		;

	/* check for bad name */
	if (*newname == '\0') {
		notify(eplayer, "Give it what new name?");
		return;
	}
	/* check for renaming a player */
	if ((thing->type == TYPE_THING && !OK_ASCII_THING(newname)) ||
			(thing->type != TYPE_THING && !OK_ASCII_OTHER(newname)) ) {
		notify(eplayer, "Invalid 8-bit name.");
		return;
	}
	if (!ok_name(newname)) {
		notify(eplayer, "That is not a reasonable name.");
		return;
	}

	/* everything ok, change the name */
	if (thing->name) {
		free((void *) thing->name);
	}
	thing->name = strdup(newname);
	notify(eplayer, "Name set.");
}

void
do_chown(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	const char *newowner = cmd->argv[2];
	OBJ *thing;
	OBJ *owner;

	if (!*name) {
		notify(eplayer, "You must specify what you want to take ownership of.");
		return;
	}

	thing = ematch_all(player, name);
	if (!thing) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	if (*newowner && strcmp(newowner, "me")) {
		owner = lookup_player(newowner);

		if (!owner) {
			notify(eplayer, "I couldn't find that player.");
			return;
		}
	} else {
		owner = player->owner;
	}
	if (!(eplayer->flags & EF_WIZARD) && player->owner != owner) {
		notify(eplayer, "Only wizards can transfer ownership to others.");
		return;
	}

	if ((eplayer->flags & EF_WIZARD) && !God(player) && God(owner)) {
		notify(eplayer, "God doesn't need an offering or sacrifice.");
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		if (!(eplayer->flags & EF_WIZARD) && player->location != thing) {
			notify(eplayer, "You can only chown \"here\".");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(eplayer->flags & EF_WIZARD) && thing->location != player) {
			notify(eplayer, "You aren't carrying that.");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_ENTITY:
		notify(eplayer, "Entities always own themselves.");
		return;
	case TYPE_GARBAGE:
		notify(eplayer, "No one wants to own garbage.");
		return;
	}
	if (owner == player)
		notify(eplayer, "Owner changed to you.");
	else {
		char buf[BUFFER_LEN];

		snprintf(buf, sizeof(buf), "Owner changed to %s.", unparse(player, owner));
		notify(eplayer, buf);
	}
}
