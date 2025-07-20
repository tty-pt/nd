#include "uapi/io.h"

#include "mcp.h"
#include "player.h"
#include "uapi/entity.h"
#include "uapi/match.h"
#include "uapi/wts.h"
#include "uapi/type.h"

void
do_examine(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	char *name = argv[1];
	unsigned thing_ref;

	if (*name == '\0') {
		thing_ref = player.location;
	} else if ((thing_ref = ematch_all(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ thing, owner;
	qdb_get(obj_hd, &thing, &thing_ref);
	qdb_get(obj_hd, &owner, &thing.owner);

	if (!controls(player_ref, thing_ref)) {
		nd_writef(player_ref, "Owner: %s\n", owner.name);
		return;
	}

	nd_writef(player_ref, "%s (#%d) Owner: %s  Value: %d\n",
			unparse(thing_ref), thing_ref, owner.name, thing.value);

	/* show him the contents */
	qdb_cur_t c = qdb_iter(contents_hd, &thing_ref);
	unsigned content_ref;

	while (qdb_next(&thing_ref, &content_ref, &c))
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

	SIC_CALL(NULL, sic_examine, player_ref, thing_ref);
}


void
do_inventory(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	OBJ player;

	look_at(player_ref, player_ref);
	qdb_get(obj_hd, &player, &player_ref);
	nd_writef(player_ref, "You have %d %s.\n", player.value,
			wts_cond("shekel", player.value));
}

void
do_owned(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), victim_ref, oi_ref;
	char *name = argv[1];
	int total = 0;

	if ((ent_get(player_ref).flags & EF_WIZARD) && *name) {
		victim_ref = player_get(name);
		if (victim_ref == NOTHING) {
			nd_writef(player_ref, NOMATCH_MESSAGE);
			return;
		}
	} else
		victim_ref = player_ref;

	OBJ victim, oi;
	qdb_get(obj_hd, &victim, &victim_ref);
	qdb_cur_t c = qdb_iter(obj_hd, NULL);
	while (qdb_next(&oi_ref, &oi, &c))
		if (oi.owner == victim.owner) {
			nd_writef(player_ref, "%s\n", unparse(oi_ref));
			total++;
		}
	nd_writef(player_ref, "%d objects found.\n", total);
}
