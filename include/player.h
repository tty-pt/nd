#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"

#include <qhash.h>

extern long player_hd;

OBJ *player_connect(const char *qsession);
OBJ *player_create(char *name);

static inline void
players_init()
{
	player_hd = hash_init();
}

static inline OBJ *
player_get(char *name)
{
	return (OBJ *) hash_sget(player_hd, name);
}

static inline void
player_put(OBJ *player)
{
	hash_sput(player_hd, player->name, player);
}

static inline void
player_delete(OBJ *player)
{
	hash_sdel(player_hd, player->name);
}

#endif
