/* $Header$ */


#include "copyright.h"
#include "config.h"

/* rob and give */

#include "mdb.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"

void
do_give(command_t *cmd)
{
	dbref player = cmd->player;
	const char *recipient = cmd->argv[1];
	int amount = atoi(cmd->argv[2]);
	dbref who;

	if (amount < 0 && !(ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD)) {
		notify(player, "Invalid amount.");
		return;
	}

	if (
			(who = ematch_me(player, recipient)) == NOTHING
			&& (who = ematch_near(player, recipient)) == NOTHING
	   )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (!(ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD)) {
		if (OBJECT(who)->type != TYPE_ENTITY) {
			notify(player, "You can only give to other entities.");
			return;
		} else if (OBJECT(who)->value + amount > MAX_PENNIES) {
			notifyf(player, "That player doesn't need that many %s!", PENNIES);
			return;
		}
	}

	if (!payfor(player, amount)) {
		notifyf(player, "You don't have that many %s to give!", PENNIES);
		return;
	}

	if (OBJECT(who)->type != TYPE_ENTITY) {
		notifyf(player, "You can't give %s to that!", PENNIES);
		return;
	}

	OBJECT(who)->value += amount;

	if (amount >= 0) {
		notifyf(player, "You give %d %s to %s.",
				amount, amount == 1 ? PENNY : PENNIES, OBJECT(who)->name);

		notifyf(who, "%s gives you %d %s.",
				OBJECT(player)->name, amount, amount == 1 ? PENNY : PENNIES);
	} else {
		notifyf(player, "You take %d %s from %s.",
				-amount, amount == -1 ? PENNY : PENNIES, OBJECT(who)->name);
		notifyf(who, "%s takes %d %s from you!",
				OBJECT(player)->name, -amount, -amount == 1 ? PENNY : PENNIES);
	}
}
