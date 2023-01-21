/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include <string.h>
#include "object.h"

#define NOMATCH_MESSAGE "I don't know what you mean."

OBJ *ematch_at(OBJ *player, OBJ *where, const char *name);
OBJ *ematch_player(const char *name);
OBJ *ematch_absolute(const char *name);
OBJ *ematch_me(OBJ *player, const char *str);
OBJ *ematch_here(OBJ *player, const char *str);
OBJ *ematch_mine(OBJ *player, const char *str);
OBJ *ematch_near(OBJ *player, const char *str);
OBJ *ematch_all(OBJ *player, const char *name);

#endif /* _MATCH_H */
