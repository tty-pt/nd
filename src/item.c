#include <stdlib.h>
#include "io.h"
#include <ndc.h>

#include "entity.h"
#include "mob.h"
#include "match.h"

void
do_select(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	const char *n_s = argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	ENT_SETF(player_ref, select, n);
	nd_writef(player_ref, "You select %u.\n", n);
}

void
do_equip(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	char *name = argv[1];
	dbref eq_ref = ematch_mine(player_ref, name);

	if (eq_ref == NOTHING)
		nd_writef(player_ref, "You are not carrying that.\n");
	else if (equip(player_ref, eq_ref)) 
		nd_writef(player_ref, "You can't equip that.\n");
}

void
do_unequip(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	char const *name = argv[1];
	enum bodypart bp = BODYPART_ID(*name);
	dbref eq_ref;

	if ((eq_ref = unequip(player_ref, bp)) == NOTHING)
		nd_writef(player_ref, "You don't have that equipped.\n");
	else
		nd_writef(player_ref, "You unequip %s.\n", obj_get(eq_ref).name);
}
