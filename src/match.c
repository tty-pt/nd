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

void
init_match(dbref player, const char *name, struct match_data *md)
{
	md->exact_match = md->last_match = NOTHING;
	md->match_count = 0;
	md->match_who = player;
	md->match_from = player;
	md->match_name = name;
	md->longest_match = 0;
	md->match_level = 0;
}

void
match_player(struct match_data *md)
{
	dbref match;
	const char *p;

	if (*(md->match_name) == LOOKUP_TOKEN && payfor(OWNER(md->match_from), LOOKUP_COST)) {
		for (p = (md->match_name) + 1; isspace(*p); p++) ;
		if ((match = lookup_player(p)) != NOTHING) {
			md->exact_match = match;
		}
	}
}

/* returns nnn if name = #nnn, else NOTHING */
static dbref
absolute_name(struct match_data *md)
{
	dbref match;

	if (*(md->match_name) == NUMBER_TOKEN) {
		match = parse_dbref((md->match_name) + 1);
		if (match < 0 || match >= db_top) {
			return NOTHING;
		} else {
			return match;
		}
	} else {
		return NOTHING;
	}
}

void
match_absolute(struct match_data *md)
{
	dbref match;

	if ((match = absolute_name(md)) != NOTHING) {
		md->exact_match = match;
	}
}

void
match_me(struct match_data *md)
{
	if (!strcmp(md->match_name, "me")) {
		md->exact_match = md->match_who;
	}
}

void
match_here(struct match_data *md)
{
	if (!strcmp(md->match_name, "here")
		&& DBFETCH(md->match_who)->location != NOTHING) {
		md->exact_match = DBFETCH(md->match_who)->location;
	}
}

void
match_home(struct match_data *md)
{
	if (!strcmp(md->match_name, "home"))
		md->exact_match = HOME;
}

static void
match_list(dbref first, struct match_data *md)
{
	dbref absolute;
	struct mob *mob = MOB(md->match_who);
	unsigned nth = mob->select;
	mob->select = 0;

	absolute = absolute_name(md);
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

void
match_possession(struct match_data *md)
{
	match_list(DBFETCH(md->match_from)->contents, md);
}

void
match_neighbor(struct match_data *md)
{
	dbref loc = getloc(md->match_from);;

	CBUG(loc == NOTHING);

	match_list(DBFETCH(loc)->contents, md);
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

	absolute = absolute_name(md);	/* parse #nnn entries */
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
match_exit_where(dbref player, dbref loc, char *name)
{
	struct match_data md;
	CBUG(Typeof(loc) != TYPE_ROOM);
	init_match(player, name, &md);
	md.match_from = loc;
	match_exits(DBFETCH(loc)->exits, &md);
	return match_result(&md);
}

void
match_everything(struct match_data *md)
{
	match_neighbor(md);
	match_possession(md);
	match_me(md);
	match_here(md);
	if (Wizard(OWNER(md->match_from)) || Wizard(md->match_who)) {
		match_absolute(md);
		match_player(md);
	}
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
noisy_match_result(struct match_data * md)
{
	dbref match;

	switch (match = match_result(md)) {
	case NOTHING:
		notify(md->match_who, NOMATCH_MESSAGE);
		return NOTHING;
	case AMBIGUOUS:
		notify(md->match_who, AMBIGUOUS_MESSAGE);
		return NOTHING;
	default:
		return match;
	}
}

dbref
ematch_from(dbref player, dbref where, const char *name) {
	dbref what;
	struct match_data md;
	init_match(player, name, &md);
	md.match_from = where;
	match_list(DBFETCH(where)->contents, &md);
	match_absolute(&md);
	what = md.exact_match;
	if (what == NOTHING)
		return NOTHING;
	if (getloc(what) != where)
		return NOTHING;
	return what;
}
