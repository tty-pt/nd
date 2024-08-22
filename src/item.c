#include <stdlib.h>
#include "io.h"
#include <ndc.h>

#include "entity.h"
#include "mob.h"
#include "match.h"

void
do_select(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	const char *n_s = argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	ENT_TMP_SETF(object_ref(player), select, n);
	nd_writef(player, "You select %u.\n", n);
}

void
do_equip(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *name = argv[1];
	OBJ *eq = ematch_mine(player, name);

	if (!eq)
		nd_writef(player, "You are not carrying that.\n");
	else if (equip(player, eq)) 
		nd_writef(player, "You can't equip that.\n");
}

void
do_unequip(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char const *name = argv[1];
	enum bodypart bp = BODYPART_ID(*name);
	dbref eq;

	if ((eq = unequip(player, bp)) == NOTHING)
		nd_writef(player, "You don't have that equipped.\n");
	else
		nd_writef(player, "You unequip %s.\n", object_get(eq)->name);
}
