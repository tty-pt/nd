#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	OBJ *thing, *clone;
	int    cost;

	/* Perform sanity checks */

	if (!(eplayer->flags & (EF_WIZARD | EF_BUILDER))) {
		ndc_writef(fd, "That command is restricted to authorized builders.\n");
		return;
	}
	
	if (*name == '\0') {
		ndc_writef(fd, "Clone what?\n");
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
		ndc_writef(fd, "I don't know what you mean.\n");
		return;
	}

	/* Further sanity checks */

	/* things only. */
	if(thing->type != TYPE_THING) {
		ndc_writef(fd, "That is not a cloneable object.\n");
		return;
	}		
	
	/* check the name again, just in case reserved name patterns have
	   changed since the original object was created. */
	if (!ok_name(thing->name)) {
		ndc_writef(fd, "You cannot clone something with such a weird name!\n");
		return;
	}

	/* no stealing stuff. */
	if(!controls(player, thing)) {
		ndc_writef(fd, "Permission denied. (you can't clone this)\n");
		return;
	}

	/* there ain't no such lunch as a free thing. */
	cost = OBJECT_GETCOST(thing->value);
	if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	
	if (!payfor(player, cost)) {
		ndc_writef(fd, "Sorry, you don't have enough %s.\n", PENNIES);
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
		ndc_writef(fd, "%s created with number %d.\n", thing->name, object_ref(clone));
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
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	char *acost =argv[2];
	OBJ *thing;
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
	qname = buf2;
	for (; *rname && isspace(*rname); rname++) ;

	cost = atoi(qname);
	if (!(eplayer->flags & (EF_WIZARD | EF_BUILDER))) {
		ndc_writef(fd, "That command is restricted to authorized builders.\n");
		return;
	}
	if (*name == '\0') {
		ndc_writef(fd, "Create what?\n");
		return;
	} else if (!ok_name(name)) {
		ndc_writef(fd, "That's a silly name for a thing!\n");
		return;
	} else if (cost < 0) {
		ndc_writef(fd, "You can't create an object for less than nothing!\n");
		return;
	} else if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	if (!payfor(player, cost)) {
		ndc_writef(fd, "Sorry, you don't have enough %s.\n", PENNIES);
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
	ndc_writef(fd, "%s created with number %d.\n", name, object_ref(thing));
}
