/* $Header$ */


#include "copyright.h"
#include "config.h"

/* rob and give */

#include "db.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"

void
do_rob(int descr, dbref player, const char *what)
{
	dbref thing;
	char buf[BUFFER_LEN];
	struct match_data md;

	init_match(descr, player, what, TYPE_PLAYER, &md);
	match_neighbor(&md);
	match_me(&md);
	if (Wizard(OWNER(player))) {
		match_absolute(&md);
		match_player(&md);
	}
	thing = match_result(&md);

	switch (thing) {
	case NOTHING:
		notify(player, "Rob whom?");
		break;
	case AMBIGUOUS:
		notify(player, "I don't know who you mean!");
		break;
	default:
		if (Typeof(thing) != TYPE_PLAYER) {
			notify(player, "Sorry, you can only rob other players.");
		} else if (GETVALUE(thing) < 1) {
			snprintf(buf, sizeof(buf), "%s has no %s.", NAME(thing), PENNIES);
			notify(player, buf);
			snprintf(buf, sizeof(buf),
					"%s tried to rob you, but you have no %s to take.",
					NAME(player), PENNIES);
			notify(thing, buf);
		} else if (can_doit(descr, player, thing, "Your conscience tells you not to.")) {
			/* steal a penny */
			SETVALUE(OWNER(player), GETVALUE(OWNER(player)) + 1);
			DBDIRTY(player);
			SETVALUE(thing, GETVALUE(thing) - 1);
			DBDIRTY(thing);
			notify_fmt(player, "You stole a %s.", PENNY);
			snprintf(buf, sizeof(buf), "%s stole one of your %s!", NAME(player), PENNIES);
			notify(thing, buf);
		}
		break;
	}
}

void
do_give(int descr, dbref player, const char *recipient, int amount)
{
	dbref who;
	char buf[BUFFER_LEN];
	struct match_data md;

	/* do amount consistency check */
	if (amount < 0 && !Wizard(OWNER(player))) {
		notify(player, "Try using the \"rob\" command.");
		return;
	} else if (amount == 0) {
		notify_fmt(player, "You must specify a positive number of %s.", PENNIES);
		return;
	}
	/* check recipient */
	init_match(descr, player, recipient, TYPE_PLAYER, &md);
	match_neighbor(&md);
	match_me(&md);
	if (Wizard(OWNER(player))) {
		match_player(&md);
		match_absolute(&md);
	}
	switch (who = match_result(&md)) {
	case NOTHING:
		notify(player, "Give to whom?");
		return;
	case AMBIGUOUS:
		notify(player, "I don't know who you mean!");
		return;
	default:
		if (!Wizard(OWNER(player))) {
			if (Typeof(who) != TYPE_PLAYER) {
				notify(player, "You can only give to other players.");
				return;
			} else if (GETVALUE(who) + amount > MAX_PENNIES) {
				notify_fmt(player, "That player doesn't need that many %s!", PENNIES);
				return;
			}
		}
		break;
	}

	/* try to do the give */
	if (!payfor(player, amount)) {
		notify_fmt(player, "You don't have that many %s to give!", PENNIES);
	} else {
		/* he can do it */
		switch (Typeof(who)) {
		case TYPE_PLAYER:
			SETVALUE(who, GETVALUE(who) + amount);
			if(amount >= 0) {
				snprintf(buf, sizeof(buf), "You give %d %s to %s.",
						amount, amount == 1 ? PENNY : PENNIES, NAME(who));
				notify(player, buf);
				snprintf(buf, sizeof(buf), "%s gives you %d %s.",
						NAME(player), amount, amount == 1 ? PENNY : PENNIES);
				notify(who, buf);
			} else {
				snprintf(buf, sizeof(buf), "You take %d %s from %s.",
						-amount, amount == -1 ? PENNY : PENNIES, NAME(who));
				notify(player, buf);
				snprintf(buf, sizeof(buf), "%s takes %d %s from you!",
						NAME(player), -amount, -amount == 1 ? PENNY : PENNIES);
				notify(who, buf);
			}
			break;
		case TYPE_THING:
			SETVALUE(who, (GETVALUE(who) + amount));
			snprintf(buf, sizeof(buf), "You change the value of %s to %d %s.",
					NAME(who),
					GETVALUE(who), GETVALUE(who) == 1 ? PENNY : PENNIES);
			notify(player, buf);
			break;
		default:
			notify_fmt(player, "You can't give %s to that!", PENNIES);
			break;
		}
		DBDIRTY(who);
	}
}
static const char *rob_c_version = "$RCSfile$ $Revision: 1.10 $";
const char *get_rob_c_version(void) { return rob_c_version; }
