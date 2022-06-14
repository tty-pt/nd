/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include <string.h>
#include "object.h"

#define NOMATCH_MESSAGE "I don't know what you mean."

OBJ *ematch_at(OBJ *player, OBJ *where, const char *name);
OBJ *ematch_player(OBJ *player, const char *name);
OBJ *ematch_absolute(const char *name);

inline OBJ *
ematch_me(OBJ *player, const char *str)
{
	if (!strcmp(str, "me"))
		return player;
	else
		return NULL;
}

inline OBJ *
ematch_here(OBJ *player, const char *str)
{
	if (!strcmp(str, "here"))
		return player->location;
	else
		return NULL;
}

inline OBJ *
ematch_mine(OBJ *player, const char *str)
{
	return ematch_at(player, player, str);
}

inline OBJ *
ematch_near(OBJ *player, const char *str)
{
	return ematch_at(player, player->location, str);
}

/* all ematch
 * (not found by the linker if it is not static?)
 */
static inline OBJ *
ematch_all(OBJ *player, const char *name)
{
	OBJ *res;

	if (
			(res = ematch_me(player, name))
			|| (res = ematch_here(player, name))
			|| (res = ematch_absolute(name))
			|| (res = ematch_near(player, name))
			|| (res = ematch_mine(player, name))
			|| (res = ematch_player(player, name))
	   )
		return res;

	else
		return NULL;
}

#endif /* _MATCH_H */
