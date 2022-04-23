/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#include "copyright.h"
#include "command.h"
#include "mdb.h"

struct match_data {
	dbref exact_match;			/* holds result of exact match */
	dbref last_match;			/* holds result of last match */
	int match_count;			/* holds total number of inexact matches */
	dbref match_who;			/* player used for me, here, and messages */
	dbref match_from;			/* object which is being matched around */
	const char *match_name;		/* name to match */
	int longest_match;			/* longest matched string */
	int match_level;			/* the highest priority level so far */
};

/* match functions */
/* Usage: init_match(descr, player, name, type); match_this(); match_that(); ... */
/* Then get value from match_result() */

/* initialize matcher */
extern void init_match(dbref player, const char *name,
					   struct match_data *md);

/* match (LOOKUP_TOKEN)player */
extern void match_player(struct match_data *md);

/* match (NUMBER_TOKEN)number */
extern void match_absolute(struct match_data *md);

extern void match_me(struct match_data *md);

extern void match_here(struct match_data *md);

extern void match_home(struct match_data *md);

/* match something player is carrying */
extern void match_possession(struct match_data *md);

/* match something in the same room as player */
extern void match_neighbor(struct match_data *md);

/* match an exit from player's room */
extern dbref match_exit_where(dbref player, dbref loc, char *name);

/* all of the above, except only Wizards do match_absolute and match_player */
extern void match_everything(struct match_data *md);

/* return match results */
extern dbref match_result(struct match_data *md);	/* returns AMBIGUOUS for

													   multiple inexacts */
#define NOMATCH_MESSAGE "I don't see that here."
#define AMBIGUOUS_MESSAGE "I don't know which one you mean!"

extern dbref noisy_match_result(struct match_data *md);

				/* wrapper for match_result */
				/* noisily notifies player */
				/* returns matched object or NOTHING */

extern dbref ematch_from(dbref player, dbref where, const char *name);

#endif /* _MATCH_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef matchh_version
#define matchh_version
const char *match_h_version = "$RCSfile$ $Revision: 1.7 $";
#endif
#else
extern const char *match_h_version;
#endif

