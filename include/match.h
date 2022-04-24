/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include "copyright.h"
#include "command.h"
#include "mdb.h"

#define NOMATCH_MESSAGE "I don't know what you mean."

extern dbref ematch_at(dbref player, dbref where, const char *name);
extern dbref ematch_player(dbref player, const char *name);
extern dbref ematch_absolute(const char *name);
extern dbref ematch_exit_at(dbref player, dbref loc, const char *name);

inline dbref
ematch_me(dbref player, const char *str)
{
	if (!strcmp(str, "me"))
		return player;
	else
		return NOTHING;
}

inline dbref
ematch_here(dbref player, const char *str)
{
	if (!strcmp(str, "here"))
		return getloc(player);
	else
		return NOTHING;
}

inline dbref
ematch_home(const char *str)
{
	if (!strcmp(str, "home"))
		return HOME;
	else
		return NOTHING;
}

inline dbref
ematch_mine(dbref player, const char *str)
{
	return ematch_at(player, player, str);
}

inline dbref
ematch_near(dbref player, const char *str)
{
	return ematch_at(player, getloc(player), str);
}

/* all ematch minus ematch_home
 * (not found by the linker if it is not static?)
 */
static inline dbref
ematch_all(dbref player, const char *name)
{
	dbref res;

	if (
			(res = ematch_me(player, name)) == NOTHING
			&& (res = ematch_here(player, name)) == NOTHING
			&& (res = ematch_absolute(name)) == NOTHING
			&& (res = ematch_near(player, name)) == NOTHING
			&& (res = ematch_mine(player, name)) == NOTHING
			&& (res = ematch_player(player, name)) == NOTHING
	   )
		return NOTHING;

	else
		return res;
}

#endif /* _MATCH_H */
