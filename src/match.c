/* $Header$ */


#include "copyright.h"
#include "config.h"

/* Routines for parsing arguments */
#include <ctype.h>

#include "mdb.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "match.h"
#include "interface.h"
#include "externs.h"
#include "mob.h"
#include "kill.h"

dbref
ematch_player(dbref player, const char *name)
{
	dbref match;
	const char *p;

	if (*name == LOOKUP_TOKEN && payfor(OWNER(player), LOOKUP_COST)) {
		for (p = name + 1; isspace(*p); p++) ;
		if ((match = lookup_player(p)) != NOTHING) {
			return match;
		}
	}

	return NOTHING;
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
ematch_absolute(const char *name)
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

static dbref
ematch_list(dbref player, dbref first, const char *name)
{
	dbref absolute;
	struct entity *mob = ENTITY(player);
	unsigned nth = mob->select;
	mob->select = 0;

	absolute = ematch_absolute(name);
	if (!controls(OWNER(player), absolute))
		absolute = NOTHING;

	DOLIST(first, first) {
		if (first == absolute) {
			return first;
		} else if (string_match(NAME(first), name)) {
			if (nth <= 0)
				return first;
			nth--;
		}
	}

	return NOTHING;
}

dbref
ematch_at(dbref player, dbref where, const char *name) {
	dbref what;

	what = ematch_absolute(name);

	if (what != NOTHING && getloc(what) == where)
		return what;

	return ematch_list(player, db[where].contents, name);
}
