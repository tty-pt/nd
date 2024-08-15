#include "io.h"
#include <ndc.h>

#include "entity.h"

#include "defaults.h"
#include "match.h"
#include "player.h"

/* prints owner of something */
static inline void
print_owner(OBJ *player, OBJ *thing)
{
	switch (thing->type) {
	case TYPE_ENTITY:
		nd_writef(player, "%s is an entity.\n", thing->name);
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		nd_writef(player, "Owner: %s\n", thing->owner->name);
		break;
	case TYPE_GARBAGE:
		nd_writef(player, "%s is garbage.\n", thing->name);
		break;
	}
}

void
do_examine(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	OBJ *thing, *content;

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		print_owner(player, thing);
		return;
	}

	if (thing->type == TYPE_GARBAGE)
		nd_writef(player, "%s\n", unparse(player, thing));
	else
		nd_writef(player, "%s (#%d) Owner: %s  Value: %d\n",
				unparse(player, thing),
				object_ref(thing),
				thing->owner->name, thing->value);

	if (thing->description)
		nd_writef(player, thing->description);

	/* show him the contents */
	if (thing->contents) {
		nd_writef(player, "Contents:\n");
		FOR_LIST(content, thing->contents) {
			nd_writef(player, unparse(player, content));
		}
	}
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			nd_writef(player, "Exits: %hhx Doors: %hhx\n", rthing->exits, rthing->doors);
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *ething = &thing->sp.equipment;
			nd_writef(player, "equipment eqw %u msv %u.\n", ething->eqw, ething->msv);
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pthing = &thing->sp.plant;
			nd_writef(player, "plant plid %u size %u.\n", pthing->plid, pthing->size);
		}
		break;
	case TYPE_SEAT:
		{
			SEA *sthing = &thing->sp.seat;
			nd_writef(player, "seat quantity %u capacity %u.\n", sthing->quantity, sthing->capacity);
		}
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (thing->location && controls(player, thing->location))
			nd_writef(player, "Location: %s\n", unparse(player, thing->location));
		break;
	case TYPE_ENTITY:
		{
			ENT *ething = &thing->sp.entity;

			nd_writef(player, "Home: %s\n", unparse(player, object_get(ething->home)));
			nd_writef(player, "hp: %d/%d entity flags: %d\n", ething->hp, HP_MAX(ething), ething->flags);

			/* print location if player can link to it */
			if (thing->location && controls(player, thing->location))
				nd_writef(player, "Location: %s\n", unparse(player, thing->location));
		}
		break;
	default:
		/* do nothing */
		break;
	}
}


void
do_score(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);

	nd_writef(player, "You have %d %s.\n", player->value,
			player->value == 1 ? PENNY : PENNIES);
}

void
do_inventory(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);

        mcp_look(player, player);

	do_score(fd, argc, argv);
}

static void
display_objinfo(OBJ *player, OBJ *obj)
{
	nd_writef(player, "%s\n", unparse(player, obj));
}

void
do_owned(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	OBJ *victim, *oi;
	int total = 0;

	if (!payfor(player, LOOKUP_COST)) {
		nd_writef(player, "You don't have enough %s.\n", PENNIES);
		return;
	}
	if ((eplayer->flags & EF_WIZARD) && *name) {
		victim = player_get(name);
		if (!victim) {
			nd_writef(player, "I couldn't find that player.\n");
			return;
		}
	} else
		victim = player;

	FOR_ALL(oi) {
		if (oi->owner == victim->owner) {
			display_objinfo(player, oi);
			total++;
		}
	}
	nd_writef(player, "***End of List***\n");
	nd_writef(player, "%d objects found.\n", total);
}

void
do_contents(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	OBJ *oi, *thing;
	int total = 0;

	if (*name == '\0') {
		thing = player->location;
	} else if (!(thing = ematch_all(player, name))) {
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		nd_writef(player, "Permission denied. (You can't get the contents of something you don't control)\n");
		return;
	}
	FOR_LIST(oi, thing->contents) {
		display_objinfo(player, oi);
		total++;
	}
	nd_writef(player, "***End of List***\n");
	nd_writef(player, "%d objects found.\n", total);
}
