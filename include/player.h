#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "hashtable.h"

extern DB *playerdb;

OBJ *player_connect(const char *qsession);
OBJ *player_create(const char *name);

static inline void
players_init()
{
	hash_init(&playerdb);
}

static inline OBJ *
player_get(const char *name)
{
	return hash_get(playerdb, name);
}

static inline void
player_put(OBJ *player)
{
	hash_put(playerdb, player->name, player);
}

static inline void
player_delete(OBJ *player)
{
	hash_delete(playerdb, player->name);
}

#endif
