#include "io.h"

#include <stddef.h> // offsetof
#include "entity.h"
#include "mcp.h"
#include "props.h"
#include "externs.h"
#include "mdb.h"
#include "interface.h"
#include "mob.h"

void
do_select(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *n_s = cmd->argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	eplayer->select = n;
	notifyf(eplayer, "You select %u.", n);
}

void
do_equip(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	char const *name = cmd->argv[1];
	OBJ *eq = ematch_mine(player, name);

	if (!eq) {
		notify(eplayer, "You are not carrying that.");
		return;
	}

	if (equip(player, eq)) { 
		notify(eplayer, "You can't equip that.");
		return;
	}
}

void
do_unequip(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	char const *name = cmd->argv[1];
	enum bodypart bp = BODYPART_ID(*name);
	dbref eq;

	if ((eq = unequip(player, bp)) == NOTHING) {
		notify(eplayer, "You don't have that equipped.");
		return;
	}

	notifyf(eplayer, "You unequip %s.", object_get(eq)->name);
}
