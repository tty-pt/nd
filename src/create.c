/* $Header$ */

#include "io.h"
#include "entity.h"
#include "config.h"

/* Commands that create new objects */

#include <string.h>
#include <ctype.h>
#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"
#include "match.h"

/*
 * do_clone
 *
 * Use this to clone an object.
 * TODO improve/remove this. use skeletons to copy objects?
 */
void
do_clone(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	char *name = cmd->argv[1];
	static char buf[BUFFER_LEN];
	OBJ *thing, *clone;
	int    cost;

	/* Perform sanity checks */

	if (!(eplayer->flags & (EF_WIZARD | EF_BUILDER))) {
		notify(eplayer, "That command is restricted to authorized builders.");
		return;
	}
	
	if (*name == '\0') {
		notify(eplayer, "Clone what?");
		return;
	} 

	/* All OK so far, so try to find the thing that should be cloned. We
	   do not allow rooms, exits, etc. to be cloned for now. */

	if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_mine(player, name))
			&& !(thing = ematch_near(player, name))
	   )
	{
		notify(eplayer, "I don't know what you mean.");
		return;
	}

	/* Further sanity checks */

	/* things only. */
	if(thing->type != TYPE_THING) {
		notify(eplayer, "That is not a cloneable object.");
		return;
	}		
	
	/* check the name again, just in case reserved name patterns have
	   changed since the original object was created. */
	if (!ok_name(thing->name)) {
		notify(eplayer, "You cannot clone something with such a weird name!");
		return;
	}

	/* no stealing stuff. */
	if(!controls(player, thing)) {
		notify(eplayer, "Permission denied. (you can't clone this)");
		return;
	}

	/* there ain't no such lunch as a free thing. */
	cost = OBJECT_GETCOST(thing->value);
	if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	
	if (!payfor(player, cost)) {
		notifyf(eplayer, "Sorry, you don't have enough %s.", PENNIES);
		return;
	} else {
		/* create the object */
		clone = object_new();

		/* initialize everything */
		clone->name = strdup(thing->name);
		clone->location = player;
		clone->owner = player->owner;
		clone->value = thing->value;
		/* FIXME: should we clone attached actions? */
		switch (thing->type) {
			case TYPE_ROOM:
				{
					ROO *rclone = &clone->sp.room;
					rclone->exits = rclone->doors = 0;
				}
				break;
			case TYPE_ENTITY:
				{
					ENT *ething = &thing->sp.entity;
					ENT *eclone = &clone->sp.entity;
					eclone->home = ething->home;
				}
				break;
		}
		clone->type = thing->type;

		/* endow the object */
		if (thing->value > MAX_OBJECT_ENDOWMENT)
			thing->value = MAX_OBJECT_ENDOWMENT;
		
		/* link it in */
		PUSH(clone, player->contents);

		/* and we're done */
		snprintf(buf, sizeof(buf), "%s created with number %d.", thing->name, object_ref(clone));
		notify(eplayer, buf);
	}
	
}

/*
 * do_create
 *
 * Use this to create an object.
 */
void
do_create(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	char *name = cmd->argv[1];
	char *acost = cmd->argv[2];
	OBJ *thing;
	int cost;

	static char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *rname, *qname;

	strlcpy(buf2, acost, sizeof(buf2));
	for (rname = buf2; (*rname && (*rname != '=')); rname++) ;
	qname = rname;
	if (*rname)
		*(rname++) = '\0';
	while ((qname > buf2) && (isspace(*qname)))
		*(qname--) = '\0';
	qname = buf2;
	for (; *rname && isspace(*rname); rname++) ;

	cost = atoi(qname);
	if (!(eplayer->flags & (EF_WIZARD | EF_BUILDER))) {
		notify(eplayer, "That command is restricted to authorized builders.");
		return;
	}
	if (*name == '\0') {
		notify(eplayer, "Create what?");
		return;
	} else if(!OK_ASCII_THING(name)) {
		notify(eplayer, "Thing names are limited to 7-bit ASCII.");
		return;
	} else if (!ok_name(name)) {
		notify(eplayer, "That's a silly name for a thing!");
		return;
	} else if (cost < 0) {
		notify(eplayer, "You can't create an object for less than nothing!");
		return;
	} else if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	if (!payfor(player, cost)) {
		notifyf(eplayer, "Sorry, you don't have enough %s.", PENNIES);
		return;
	}

	/* create the object */
	thing = object_new();

	/* initialize everything */
	thing->name = strdup(name);
	thing->location = player;
	thing->owner = player->owner;
	thing->value = OBJECT_ENDOWMENT(cost);
	thing->type = TYPE_THING;

	/* endow the object */
	if (thing->value > MAX_OBJECT_ENDOWMENT)
		thing->value = MAX_OBJECT_ENDOWMENT;

	/* link it in */
	PUSH(thing, player->contents);

	/* and we're done */
	snprintf(buf, sizeof(buf), "%s created with number %d.", name, object_ref(thing));
	notify(eplayer, buf);
}
