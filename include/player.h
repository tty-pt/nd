#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"

#include <hash.h>

extern int player_hd;

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
	return SHASH_GET(player_hd, name);
}

static inline void
player_put(OBJ *player)
{
	SHASH_PUT(player_hd, player->name, player);
}

static inline void
player_delete(OBJ *player)
{
	SHASH_DEL(player_hd, player->name);
}

#endif
