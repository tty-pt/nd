#include "io.h"
#include <ndc.h>

#include "entity.h"

#include "defaults.h"
#include "match.h"
#include "player.h"

/* prints owner of something */
static inline void
print_owner(int fd, OBJ *thing)
{
	switch (thing->type) {
	case TYPE_ENTITY:
		ndc_writef(fd, "%s is an entity.\n", thing->name);
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		ndc_writef(fd, "Owner: %s\n", thing->owner->name);
		break;
	case TYPE_GARBAGE:
		ndc_writef(fd, "%s is garbage.\n", thing->name);
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
		ndc_writef(fd, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		print_owner(fd, thing);
		return;
	}

	if (thing->type == TYPE_GARBAGE)
		ndc_writef(fd, "%s\n", unparse(player, thing));
	else
		ndc_writef(fd, "%s (#%d) Owner: %s  Value: %d\n",
				unparse(player, thing),
				object_ref(thing),
				thing->owner->name, thing->value);

	if (thing->description)
		ndc_writef(fd, thing->description);

	/* show him the contents */
	if (thing->contents) {
		ndc_writef(fd, "Contents:\n");
		FOR_LIST(content, thing->contents) {
			ndc_writef(fd, unparse(player, content));
		}
	}
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			ndc_writef(fd, "Exits: %hhx Doors: %hhx\n", rthing->exits, rthing->doors);

			if (rthing->dropto)
				ndc_writef(fd, "Dropped objects go to: %s\n",
						unparse(player, rthing->dropto));
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *ething = &thing->sp.equipment;
			ndc_writef(fd, "equipment eqw %u msv %u.\n", ething->eqw, ething->msv);
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pthing = &thing->sp.plant;
			ndc_writef(fd, "plant plid %u size %u.\n", pthing->plid, pthing->size);
		}
		break;
	case TYPE_SEAT:
		{
			SEA *sthing = &thing->sp.seat;
			ndc_writef(fd, "seat quantity %u capacity %u.\n", sthing->quantity, sthing->capacity);
		}
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (thing->location && controls(player, thing->location))
			ndc_writef(fd, "Location: %s\n", unparse(player, thing->location));
		break;
	case TYPE_ENTITY:
		{
			ENT *ething = &thing->sp.entity;

			ndc_writef(fd, "Home: %s\n", unparse(player, ething->home));
			ndc_writef(fd, "hp: %d/%d entity flags: %d\n", ething->hp, HP_MAX(ething), ething->flags);

			/* print location if player can link to it */
			if (thing->location && controls(player, thing->location))
				ndc_writef(fd, "Location: %s\n", unparse(player, thing->location));
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

	ndc_writef(fd, "You have %d %s.\n", player->value,
			player->value == 1 ? PENNY : PENNIES);
}

void
do_inventory(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd),
	    *thing;

        if (mcp_look(player, player)) {
		thing = player->contents;
                if (!thing) {
                        ndc_writef(fd, "You aren't carrying anything.\n");
                } else {
                        ndc_writef(fd, "You are carrying:\n");
                        FOR_LIST(thing, thing) {
                                ndc_writef(fd, "%s\n", unparse(player, thing));
                        }
                }
        }

	do_score(fd, argc, argv);
}

static void
display_objinfo(OBJ *player, OBJ *obj)
{
	ENT *eplayer = &player->sp.entity;
	ndc_writef(eplayer->fd, "%s\n", unparse(player, obj));
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
		ndc_writef(fd, "You don't have enough %s.\n", PENNIES);
		return;
	}
	if ((eplayer->flags & EF_WIZARD) && *name) {
		victim = player_get(name);
		if (!victim) {
			ndc_writef(fd, "I couldn't find that player.\n");
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
	ndc_writef(fd, "***End of List***\n");
	ndc_writef(fd, "%d objects found.\n", total);
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
		ndc_writef(fd, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		ndc_writef(fd, "Permission denied. (You can't get the contents of something you don't control)\n");
		return;
	}
	FOR_LIST(oi, thing->contents) {
		display_objinfo(player, oi);
		total++;
	}
	ndc_writef(fd, "***End of List***\n");
	ndc_writef(fd, "%d objects found.\n", total);
}
