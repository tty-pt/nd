/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include <string.h>
#include "object.h"

#define NOMATCH_MESSAGE "I don't know what you mean.\n"

OBJ *ematch_at(OBJ *player, OBJ *where, char *name);
OBJ *ematch_player(char *name);
OBJ *ematch_absolute(char *name);
OBJ *ematch_me(OBJ *player, char *str);
OBJ *ematch_here(OBJ *player, char *str);
OBJ *ematch_mine(OBJ *player, char *str);
OBJ *ematch_near(OBJ *player, char *str);
OBJ *ematch_all(OBJ *player, char *name);

#endif /* _MATCH_H */
