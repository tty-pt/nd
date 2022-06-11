
/* $Header$ */


#include "copyright.h"
#include "config.h"

/* commands which set parameters */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "props.h"
#include "match.h"
#include "interface.h"
#include "externs.h"
#include "geography.h"


static OBJ *
match_controlled(OBJ *player, const char *name)
{
	OBJ *match = ematch_all(player, name);

	if (!match) {
		notify(player, NOMATCH_MESSAGE);
		return NULL;
	}

	else if (!controls(player, match)) {
		notify(player, "Permission denied. (You don't control what was matched)");
		return NULL;
	}

	return match;
}

void
do_name(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	char *newname = cmd->argv[2];
	OBJ *thing = match_controlled(player, name);

	if (!thing)
		return;
		;

	/* check for bad name */
	if (*newname == '\0') {
		notify(player, "Give it what new name?");
		return;
	}
	/* check for renaming a player */
	if ((thing->type == TYPE_THING && !OK_ASCII_THING(newname)) ||
			(thing->type != TYPE_THING && !OK_ASCII_OTHER(newname)) ) {
		notify(player, "Invalid 8-bit name.");
		return;
	}
	if (!ok_name(newname)) {
		notify(player, "That is not a reasonable name.");
		return;
	}

	/* everything ok, change the name */
	if (thing->name) {
		free((void *) thing->name);
	}
	thing->name = alloc_string(newname);
	notify(player, "Name set.");
}

void
do_chown(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	const char *newowner = cmd->argv[2];
	OBJ *thing;
	OBJ *owner;

	if (!*name) {
		notify(player, "You must specify what you want to take ownership of.");
		return;
	}

	thing = ematch_all(player, name);
	if (!thing) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (*newowner && strcmp(newowner, "me")) {
		owner = lookup_player(newowner);

		if (!owner) {
			notify(player, "I couldn't find that player.");
			return;
		}
	} else {
		owner = player->owner;
	}
	if (!(eplayer->flags & EF_WIZARD) && player->owner != owner) {
		notify(player, "Only wizards can transfer ownership to others.");
		return;
	}
#ifdef GOD_PRIV
	if ((eplayer->flags & EF_WIZARD) && !God(player) && God(owner)) {
		notify(player, "God doesn't need an offering or sacrifice.");
		return;
	}
#endif /* GOD_PRIV */

	switch (thing->type) {
	case TYPE_ROOM:
		if (!(eplayer->flags & EF_WIZARD) && player->location != thing) {
			notify(player, "You can only chown \"here\".");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(eplayer->flags & EF_WIZARD) && thing->location != player) {
			notify(player, "You aren't carrying that.");
			return;
		}
		thing->owner = owner->owner;
		break;
	case TYPE_ENTITY:
		notify(player, "Entities always own themselves.");
		return;
	case TYPE_GARBAGE:
		notify(player, "No one wants to own garbage.");
		return;
	}
	if (owner == player)
		notify(player, "Owner changed to you.");
	else {
		char buf[BUFFER_LEN];

		snprintf(buf, sizeof(buf), "Owner changed to %s.", unparse_object(player, owner));
		notify(player, buf);
	}
}


void
do_propset(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	const char *prop = cmd->argv[2];
	OBJ *thing = match_controlled(player, name),
	    *ref;
	char *p, *q;
	char buf[BUFFER_LEN];
	char *type, *pname, *value;
	PData mydat;

	/* find thing */
	if (!thing)
		return;

	while (isspace(*prop))
		prop++;
	strlcpy(buf, prop, sizeof(buf));

	type = p = buf;
	while (*p && *p != PROP_DELIMITER)
		p++;
	if (*p)
		*p++ = '\0';

	if (*type) {
		q = type + strlen(type) - 1;
		while (q >= type && isspace(*q))
			*(q--) = '\0';
	}

	pname = p;
	while (*p && *p != PROP_DELIMITER)
		p++;
	if (*p)
		*p++ = '\0';
	value = p;

	while (*pname == PROPDIR_DELIMITER || isspace(*pname))
		pname++;
	if (*pname) {
		q = pname + strlen(pname) - 1;
		while (q >= pname && isspace(*q))
			*(q--) = '\0';
	}

	if (!*pname) {
		notify(player, "I don't know which property you want to set!");
		return;
	}

	ENT *eplayer = &player->sp.entity;

	if (Prop_System(pname) || (!(eplayer->flags & EF_WIZARD) && (Prop_SeeOnly(pname) || Prop_Hidden(pname)))) {
		notify(player, "Permission denied. (can't set a property that's restricted against you)");
		return;
	}

	if (!*type || string_prefix("string", type))
		add_prop_nofetch(thing, pname, value, 0);
	else if (string_prefix("integer", type)) {
		if (!number(value)) {
			notify(player, "That's not an integer!");
			return;
		}
		add_prop_nofetch(thing, pname, NULL, atoi(value));
	} else if (string_prefix("float", type)) {
		if (!ifloat(value)) {
			notify(player, "That's not a floating point number!");
			return;
		}
		mydat.flags = PROP_FLTTYP;
		mydat.data.fval = strtod(value, NULL);
		set_property_nofetch(thing, pname, &mydat);
	} else if (string_prefix("dbref", type)) {
		ref = ematch_all(player, value);
		if (!ref) {
			notify(player, NOMATCH_MESSAGE);
			return;
		}

		mydat.flags = PROP_REFTYP;
		mydat.data.ref = object_ref(ref);
		set_property_nofetch(thing, pname, &mydat);
	} else if (string_prefix("erase", type)) {
		if (*value) {
			notify(player, "Don't give a value when erasing a property.");
			return;
		}
		remove_property(thing, pname);
		notify(player, "Property erased.");
		return;
	} else {
		notify(player, "I don't know what type of property you want to set!");
		notify(player, "Valid types are string, integer, float, dbref, lock, and erase.");
		return;
	}
	notify(player, "Property set.");
}

static const char *set_c_version = "$RCSfile$ $Revision: 1.31 $";
const char *get_set_c_version(void) { return set_c_version; }
