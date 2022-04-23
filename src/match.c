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

struct match_data {
	dbref exact_match;			/* holds result of exact match */
	dbref last_match;			/* holds result of last match */
	int match_count;			/* holds total number of inexact matches */
	dbref match_who;			/* player used for me, here, and messages */
	dbref match_from;			/* object which is being matched around */
	const char *match_name;		/* name to match */
	int longest_match;			/* longest matched string */
};

static void
init_match(dbref player, const char *name, struct match_data *md)
{
	md->exact_match = md->last_match = NOTHING;
	md->match_count = 0;
	md->match_who = player;
	md->match_from = player;
	md->match_name = name;
	md->longest_match = 0;
}

dbref
match_result(struct match_data *md)
{
	if (md->exact_match != NOTHING) {
		return (md->exact_match);
	} else {
		switch (md->match_count) {
		case 0:
			return NOTHING;
		case 1:
			return (md->last_match);
		default:
			return AMBIGUOUS;
		}
	}
}

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

void
match_absolute(struct match_data *md)
{
	dbref match;

	if ((match = ematch_absolute(md->match_name)) != NOTHING) {
		md->exact_match = match;
	}
}

static void
match_list(dbref first, struct match_data *md)
{
	dbref absolute;
	struct mob *mob = MOB(md->match_who);
	unsigned nth = mob->select;
	mob->select = 0;

	absolute = ematch_absolute(md->match_name);
	if (!controls(OWNER(md->match_from), absolute))
		absolute = NOTHING;

	DOLIST(first, first) {
		if (first == absolute) {
			md->exact_match = first;
			return;
		} else if (string_match(NAME(first), md->match_name)) {
			if (md->exact_match <= 0 && nth <= 0)
				md->exact_match = first;
			nth--;
			md->last_match = first;
			(md->match_count)++;
		}
	}
}

/*
 * match_exits matches a list of exits, starting with 'first'.
 * It will match exits of players, rooms, or things.
 */
static void
match_exits(dbref first, struct match_data *md)
{
	dbref exit, absolute;
	const char *exitname, *p;

	if (first == NOTHING)
		return;					/* Easy fail match */
	if (getloc(md->match_from) == NOTHING)
		return;

	absolute = ematch_absolute(md->match_name);	/* parse #nnn entries */
	if (!controls(OWNER(md->match_from), absolute))
		absolute = NOTHING;

	DOLIST(exit, first) {
		if (exit == absolute) {
			md->exact_match = exit;
			continue;
		}
		exitname = NAME(exit);
		while (*exitname) {		/* for all exit aliases */
			for (p = md->match_name;	/* check out 1 alias */
				 *p &&
				 tolower(*p) == tolower(*exitname) &&
				 *exitname != EXIT_DELIMITER;
				 p++, exitname++);

			/* did we get a match on this alias? */
			if (*p == '\0') {
				/* make sure there's nothing afterwards */
				while (isspace(*exitname))
					exitname++;
				CBUG(getloc(exit) == NOTHING);
				CBUG(Typeof(getloc(exit)) != TYPE_ROOM);
				if (*exitname == '\0' || *exitname == EXIT_DELIMITER) {
					/* we got a match on this alias */
					if (strlen(md->match_name) - strlen(p) > md->longest_match) {
						md->exact_match = exit;
						md->longest_match = strlen(md->match_name) - strlen(p);
					}
					goto next_exit;
				}
			}
			/* we didn't get it, go on to next alias */
			while (*exitname && *exitname++ != EXIT_DELIMITER) ;
			while (isspace(*exitname))
				exitname++;
		}						/* end of while alias string matches */
	  next_exit:
		;
	}
}

dbref
ematch_exit_at(dbref player, dbref loc, const char *name)
{
	struct match_data md;
	CBUG(Typeof(loc) != TYPE_ROOM);
	init_match(player, name, &md);
	md.match_from = loc;
	match_exits(DBFETCH(loc)->exits, &md);
	return match_result(&md);
}

dbref
ematch_at(dbref player, dbref where, const char *name) {
	dbref what;
	struct match_data md;
	what = ematch_absolute(name);
	if (what != NOTHING && getloc(what) == where)
		return what;
	init_match(player, name, &md);
	md.match_from = where;
	match_list(DBFETCH(where)->contents, &md);
	what = md.exact_match;
	return what;
}
