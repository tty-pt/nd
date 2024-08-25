/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include <string.h>
#include "object.h"

#define NOMATCH_MESSAGE "I don't know what you mean.\n"

dbref ematch_at(dbref player_ref, dbref where_ref, char *name);
dbref ematch_player(char *name);
dbref ematch_absolute(char *name);
dbref ematch_me(dbref player_ref, char *str);
dbref ematch_here(dbref player_ref, char *str);
dbref ematch_mine(dbref player_ref, char *str);
dbref ematch_near(dbref player_ref, char *str);
dbref ematch_all(dbref player_ref, char *name);

#endif /* _MATCH_H */
