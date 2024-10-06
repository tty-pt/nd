#include "uapi/io.h"

#include "mcp.h"
#include "player.h"
#include "uapi/entity.h"
#include "uapi/match.h"
#include "uapi/wts.h"

/* prints owner of something */
static inline void
print_owner(unsigned player_ref, unsigned thing_ref)
{
	OBJ thing, owner;

	lhash_get(obj_hd, &thing, thing_ref);
	switch (thing.type) {
	case TYPE_ENTITY:
		nd_writef(player_ref, "%s is an entity.\n", thing.name);
		break;
	case TYPE_ROOM:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		lhash_get(obj_hd, &owner, thing.owner);
		nd_writef(player_ref, "Owner: %s\n", owner.name);
		break;
	}
}

void
do_examine(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	char *name = argv[1];
	unsigned thing_ref;

	if (*name == '\0') {
		thing_ref = player.location;
	} else if ((thing_ref = ematch_all(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player_ref, thing_ref)) {
		print_owner(player_ref, thing_ref);
		return;
	}

	OBJ thing, thing_owner;
	lhash_get(obj_hd, &thing, thing_ref);
	lhash_get(obj_hd, &thing_owner, thing.owner);

	nd_writef(player_ref, "%s (#%d) Owner: %s  Value: %d\n",
			unparse(thing_ref),
			thing_ref,
			thing_owner.name, thing.value);

	if (thing.description)
		nd_writef(player_ref, thing.description);

	/* show him the contents */
	struct hash_cursor c = fhash_iter(contents_hd, thing_ref);
	unsigned content_ref;

	while (ahash_next(&content_ref, &c))
		nd_writef(player_ref, unparse(content_ref));

	switch (thing.type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing.sp.room;
			nd_writef(player_ref, "Exits: %hhx Doors: %hhx\n", rthing->exits, rthing->doors);
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *ething = &thing.sp.equipment;
			nd_writef(player_ref, "equipment eqw %u msv %u.\n", ething->eqw, ething->msv);
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pthing = &thing.sp.plant;
			nd_writef(player_ref, "plant plid %u size %u.\n", pthing->plid, pthing->size);
		}
		break;
	case TYPE_SEAT:
		{
			SEA *sthing = &thing.sp.seat;
			nd_writef(player_ref, "seat quantity %u capacity %u.\n", sthing->quantity, sthing->capacity);
		}
		break;
	case TYPE_THING:
		/* print location if player can link to it */
		if (thing.location != NOTHING && controls(player_ref, thing.location))
			nd_writef(player_ref, "Location: %s\n", unparse(thing.location));
		break;
	case TYPE_ENTITY:
		{
			ENT ething = ent_get(thing_ref);

			nd_writef(player_ref, "Home: %s\n", unparse(ething.home));
			nd_writef(player_ref, "hp: %d/%d entity flags: %d\n", ething.hp, HP_MAX(&ething), ething.flags);

			/* print location if player can link to it */
			if (thing.location != NOTHING && controls(player_ref, thing.location))
				nd_writef(player_ref, "Location: %s\n", unparse(thing.location));
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
	unsigned player_ref = fd_player(fd);
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);

	nd_writef(player_ref, "You have %d %s.\n", player.value,
			wts_cond("shekel", player.value));
}

void
do_inventory(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);

        mcp_look(player_ref, player_ref);
	do_score(fd, argc, argv);
}

static void
display_objinfo(unsigned player_ref, unsigned obj_ref)
{
	nd_writef(player_ref, "%s\n", unparse(obj_ref));
}

void
do_owned(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), victim_ref, oi_ref;
	char *name = argv[1];
	int total = 0;

	if ((ent_get(player_ref).flags & EF_WIZARD) && *name) {
		victim_ref = player_get(name);
		if (victim_ref == NOTHING) {
			nd_writef(player_ref, "I couldn't find that player.\n");
			return;
		}
	} else
		victim_ref = player_ref;

	OBJ victim, oi;
	lhash_get(obj_hd, &victim, victim_ref);
	struct hash_cursor c = lhash_iter(obj_hd);
	while (hash_next(&oi_ref, &oi, &c))
		if (oi.owner == victim.owner) {
			display_objinfo(player_ref, oi_ref);
			total++;
		}
	nd_writef(player_ref, "***End of List***\n");
	nd_writef(player_ref, "%d objects found.\n", total);
}

void
do_contents(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	char *name = argv[1];
	int total = 0;

	if (*name == '\0') {
		OBJ player;
		lhash_get(obj_hd, &player, player_ref);
		thing_ref = player.location;
	} else if ((thing_ref = ematch_all(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player_ref, thing_ref)) {
		nd_writef(player_ref, "Permission denied. (You can't get the contents of something you don't control)\n");
		return;
	}

	struct hash_cursor c = fhash_iter(contents_hd, thing_ref);
	unsigned oi_ref;
	while (ahash_next(&oi_ref, &c)) {
		display_objinfo(player_ref, oi_ref);
		total++;
	}
	nd_writef(player_ref, "***End of List***\n");
	nd_writef(player_ref, "%d objects found.\n", total);
}
