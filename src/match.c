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

OBJ *
ematch_me(OBJ *player, char *str)
{
	if (!strcmp(str, "me"))
		return player;
	else
		return NULL;
}

OBJ *
ematch_here(OBJ *player, char *str)
{
	if (!strcmp(str, "here"))
		return object_get(player->location);
	else
		return NULL;
}

OBJ *
ematch_mine(OBJ *player, char *str)
{
	return ematch_at(player, player, str);
}

OBJ *
ematch_near(OBJ *player, char *str)
{
	return ematch_at(player, object_get(player->location), str);
}

/* all ematch
 * (not found by the linker if it is not static?)
 */
OBJ *
ematch_all(OBJ *player, char *name)
{
	OBJ *res;

	if (
			(res = ematch_me(player, name))
			|| (res = ematch_here(player, name))
			|| (res = ematch_absolute(name))
			|| (res = ematch_near(player, name))
			|| (res = ematch_mine(player, name))
			|| (res = ematch_player(name))
	   )
		return res;

	else
		return NULL;
}


/* TODO move to match.h as static inline */
OBJ *
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
OBJ *
ematch_absolute(char *name)
{
	dbref match;
	if (*name == NUMBER_TOKEN) {
		match = parse_dbref(name + 1);
		if (match < 0 || match >= db_top)
			return NULL;
		else
			return object_get(match);
	} else
		return NULL;
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

OBJ *
ematch_at(OBJ *player, OBJ *where, char *name) {
	OBJ *what, *tmp;

	what = ematch_absolute(name);

	if (what && what->location == object_ref(where))
		return what;

	OBJ *absolute;
	dbref ref = object_ref(player);
	struct ent_tmp *etmp = ent_tmp_get(ref);
	unsigned nth = etmp->select;
	etmp->select = 0;
	ent_tmp_set(ref, etmp);

	absolute = ematch_absolute(name);
	if (!controls(player, absolute))
		absolute = NULL;

	struct hash_cursor c = contents_iter(object_ref(where));
	while ((tmp = contents_next(&c))) {
		if (tmp == absolute) {
			return tmp;
		} else if (string_match(tmp->name, name)) {
			if (nth <= 0)
				return tmp;
			nth--;
		}
	}

	return NULL;
}
