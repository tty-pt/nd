#include "uapi/io.h"

#include <stdlib.h>

#include "uapi/entity.h"
#include "uapi/match.h"

#define BODYPART_ID(_c) ch_bodypart_map[(int) _c]

enum bodypart ch_bodypart_map[] = {
	[0 ... 254] = 0,
	['h'] = BP_HEAD,
	['n'] = BP_NECK,
	['c'] = BP_CHEST,
	['b'] = BP_BACK,
	['w'] = BP_WEAPON,
	['l'] = BP_LFINGER,
	['r'] = BP_RFINGER,
	['g'] = BP_LEGS,
};

void
do_select(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	const char *n_s = argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	ENT eplayer = ent_get(player_ref);
	eplayer.select = n;
	ent_set(player_ref, &eplayer);
	nd_writef(player_ref, "You select %u.\n", n);
}

void
do_equip(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	unsigned eq_ref = ematch_mine(player_ref, name);

	if (eq_ref == NOTHING)
		nd_writef(player_ref, "You are not carrying that.\n");
	else if (equip(player_ref, eq_ref)) 
		nd_writef(player_ref, "You can't equip that.\n");
}

void
do_unequip(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	char const *name = argv[1];
	enum bodypart bp = BODYPART_ID(*name);
	unsigned eq_ref;

	if ((eq_ref = unequip(player_ref, bp)) == NOTHING)
		nd_writef(player_ref, "You don't have that equipped.\n");
	else {
		OBJ eq;
		lhash_get(obj_hd, &eq, eq_ref);
		nd_writef(player_ref, "You unequip %s.\n", eq.name);
	}
}
