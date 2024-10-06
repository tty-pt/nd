#ifndef PLAYER_H
#define PLAYER_H

#include "uapi/object.h"

#include <string.h>
#include <qhash.h>

extern unsigned player_hd;

OBJ *player_connect(const char *qsession);
OBJ *player_create(char *name);

static inline void
players_init()
{
	player_hd = hash_init();
}

static inline unsigned
player_get(char *name)
{
	unsigned res = NOTHING;
	shash_get(player_hd, &res, name);
	return res;
}

static inline void
player_put(char *name, unsigned player_ref)
{
	suhash_put(player_hd, name, player_ref);
}

static inline void
player_delete(char *name)
{
	shash_del(player_hd, name);
}

#endif
