/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include "copyright.h"
#include "command.h"
#include "mdb.h"

enum match {
	MCH_ABS = 1,
	MCH_ME = 2,
	MCH_HERE = 4,
	MCH_HOME = 8,
	MCH_PLAYER = 16,
	MCH_MINE = 32,
	MCH_NEAR = 64,
	MCH_ALL = 1 | 2 | 4 | 32 | 64, /* not really */
};

extern dbref ematch(dbref player, const char *name, unsigned char flags);
extern dbref ematch_noisy(dbref player, const char *name, unsigned char flags);
extern dbref ematch_at(dbref player, dbref where, const char *name);
extern dbref ematch_exit_at(dbref player, dbref loc, const char *name);

#define NOMATCH_MESSAGE "I don't see that here."
#define AMBIGUOUS_MESSAGE "I don't know which one you mean!"

#endif /* _MATCH_H */
