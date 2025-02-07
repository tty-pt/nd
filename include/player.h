#ifndef PLAYER_H
#define PLAYER_H

#include "uapi/object.h"
#include "config.h"

#include <string.h>
#include <qdb.h>

extern unsigned player_hd;

OBJ *player_connect(const char *qsession);
OBJ *player_create(char *name);

static inline unsigned
player_get(char *name)
{
	unsigned res = NOTHING;
	qdb_get(player_hd, &res, name);
	return res;
}

static inline void
player_put(char *name, unsigned player_ref)
{
	qdb_put(player_hd, name, &player_ref);
}

static inline void
player_delete(char *name)
{
	qdb_del(player_hd, name, NULL);
}

#endif
