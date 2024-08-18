#include <string.h>

#include "player.h"
#include "io.h"
#include "entity.h"

#include "defaults.h"

int player_hd = -1;

OBJ *
player_create(char *name)
{
	OBJ *player = object_new();
	ENT *eplayer = &player->sp.entity;

	player->name = strdup(name);
	player->location = eplayer->home = PLAYER_START;
	player->type = TYPE_ENTITY;
	player->owner = object_ref(player);
	player->value = START_PENNIES;
	eplayer->flags = EF_PLAYER;

	PUSH(player, object_get(PLAYER_START)->contents);
	player_put(player);

	st_start(player);

	return player;
}
