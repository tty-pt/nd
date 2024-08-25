/* $Header$ */


#include "copyright.h"
#include "config.h"

#include <ctype.h>
#include <stdlib.h>

#include "params.h"
#include "match.h"
#include "utils.h"
#include "entity.h"
#include "player.h"

dbref
ematch_me(dbref player_ref, char *str)
{
	if (!strcmp(str, "me"))
		return player_ref;
	else
		return NOTHING;
}

dbref
ematch_here(dbref player_ref, char *str)
{
	if (!strcmp(str, "here"))
		return obj_get(player_ref).location;
	else
		return NOTHING;
}

dbref
ematch_mine(dbref player_ref, char *str)
{
	return ematch_at(player_ref, player_ref, str);
}

dbref
ematch_near(dbref player_ref, char *str)
{
	return ematch_at(player_ref, obj_get(player_ref).location, str);
}

/* all ematch
 * (not found by the linker if it is not static?)
 */
dbref
ematch_all(dbref player_ref, char *name)
{
	dbref res;

	if (
			(res = ematch_me(player_ref, name)) != NOTHING
			|| (res = ematch_here(player_ref, name)) != NOTHING
			|| (res = ematch_absolute(name)) != NOTHING
			|| (res = ematch_near(player_ref, name)) != NOTHING
			|| (res = ematch_mine(player_ref, name)) != NOTHING
			|| (res = ematch_player(name)) != NOTHING
	   )
		return res;

	else
		return NOTHING;
}


/* TODO move to match.h as static inline */
dbref
ematch_player(char *name)
{
	return player_get(name);
}

static dbref
parse_dbref(const char *s)
{
	const char *p;
	long x;

	x = atol(s);
	if (x > 0) {
		return x;
	} else if (x == 0) {
		/* check for 0 */
		for (p = s; *p; p++) {
			if (*p == '0')
				return 0;
			if (!isspace(*p))
				break;
		}
	}
	/* else x < 0 or s != 0 */
	return NOTHING;
}

/* returns nnn if name = #nnn, else NOTHING */
dbref
ematch_absolute(char *name)
{
	dbref match;
	if (*name == NUMBER_TOKEN) {
		match = parse_dbref(name + 1);
		if (match < 0 || match >= db_top)
			return NOTHING;
		else
			return match;
	} else
		return NOTHING;
}

/* accepts only nonempty matches starting at the beginning of a word */
static inline const char *
string_match(register const char *src, register const char *sub)
{
	if (*sub != '\0') {
		while (*src) {
			if (string_prefix(src, sub))
				return src;
			/* else scan to beginning of next word */
			while (*src && isalnum(*src))
				src++;
			while (*src && !isalnum(*src))
				src++;
		}
	}
	return 0;
}

dbref
ematch_at(dbref player_ref, dbref where_ref, char *name) {
	dbref what_ref = ematch_absolute(name),
	      absolute_ref, tmp_ref;

	if (what_ref != NOTHING && obj_get(what_ref).location == where_ref)
		return what_ref;

	ENT ent = ent_get(player_ref);
	unsigned nth = ent.select;
	ent.select = 0;
	ent_set(player_ref, &ent);

	absolute_ref = ematch_absolute(name);

	if (absolute_ref != NOTHING && !controls(player_ref, absolute_ref))
		absolute_ref = NOTHING;

	struct hash_cursor c = contents_iter(where_ref);
	while ((tmp_ref = contents_next(&c))) {
		if (tmp_ref == absolute_ref) {
			return tmp_ref;
		} else if (string_match(obj_get(tmp_ref).name, name)) {
			if (nth <= 0)
				return tmp_ref;
			nth--;
		}
	}

	return NOTHING;
}
