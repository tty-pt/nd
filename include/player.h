#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"

#include <string.h>
#include <qhash.h>

extern long player_hd;

OBJ *player_connect(const char *qsession);
OBJ *player_create(char *name);

static inline void
players_init()
{
	player_hd = hash_init();
}

static inline dbref
player_get(char *name)
{
	dbref res = NOTHING;
	hash_cget(player_hd, &res, name, strlen(name));
	return res;
}

static inline void
player_put(char *name, dbref player_ref)
{
	hash_cput(player_hd, name, strlen(name), &player_ref, sizeof(player_ref));
}

static inline void
player_delete(char *name)
{
	hash_sdel(player_hd, name);
}

#endif
