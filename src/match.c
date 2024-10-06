#include "uapi/match.h"

#include <ctype.h>
#include <stdlib.h>

#include "params.h"
#include "uapi/entity.h"
#include "player.h"

unsigned
ematch_me(unsigned player_ref, char *str)
{
	if (!strcmp(str, "me"))
		return player_ref;
	else
		return NOTHING;
}

unsigned
ematch_here(unsigned player_ref, char *str)
{
	if (!strcmp(str, "here")) {
		OBJ player;
		lhash_get(obj_hd, &player, player_ref);
		return player.location;
	} else
		return NOTHING;
}

unsigned
ematch_mine(unsigned player_ref, char *str)
{
	return ematch_at(player_ref, player_ref, str);
}

unsigned
ematch_near(unsigned player_ref, char *str)
{
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	return ematch_at(player_ref, player.location, str);
}

/* all ematch
 * (not found by the linker if it is not static?)
 */
unsigned
ematch_all(unsigned player_ref, char *name)
{
	unsigned res;

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
unsigned
ematch_player(char *name)
{
	return player_get(name);
}

static unsigned
parse_unsigned(const char *s)
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
unsigned
ematch_absolute(char *name)
{
	unsigned match;
	if (*name == NUMBER_TOKEN) {
		match = parse_unsigned(name + 1);
		if (match < 0 || !obj_exists(match))
			return NOTHING;
		else
			return match;
	} else
		return NOTHING;
}

static inline int
string_prefix(register const char *string, register const char *prefix)
{
	while (*string && *prefix && tolower(*string) == tolower(*prefix))
		string++, prefix++;
	return *prefix == '\0';
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

unsigned
ematch_at(unsigned player_ref, unsigned where_ref, char *name) {
	unsigned what_ref = ematch_absolute(name),
	      absolute_ref, tmp_ref = NOTHING;
	OBJ what;

	lhash_get(obj_hd, &what, what_ref);
	if (what_ref != NOTHING && what.location == where_ref)
		return what_ref;

	ENT ent = ent_get(player_ref);
	unsigned nth = ent.select;
	ent.select = 0;
	ent_set(player_ref, &ent);

	absolute_ref = ematch_absolute(name);

	if (absolute_ref != NOTHING && !controls(player_ref, absolute_ref))
		absolute_ref = NOTHING;

	struct hash_cursor c = fhash_iter(contents_hd, where_ref);
	while (ahash_next(&tmp_ref, &c)) {
		if (tmp_ref == absolute_ref) {
			hash_fin(&c);
			break;
		}

		OBJ tmp;
		lhash_get(obj_hd, &tmp, tmp_ref);
		if (string_match(tmp.name, name)) {
			if (nth <= 0) {
				hash_fin(&c);
				break;
			}
			nth--;
		}
	}

	return tmp_ref;
}
