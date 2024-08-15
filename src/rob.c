#include <ndc.h>
#include <stdlib.h>
#include "io.h"
#include "entity.h"
#include "params.h"
#include "defaults.h"
#include "match.h"

void
do_give(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd), who_ref;
	int wizard = ent_get(player_ref).flags & EF_WIZARD;
	char *recipient = argv[1];
	int amount = atoi(argv[2]);

	if (amount < 0 && !wizard) {
		nd_writef(player_ref, "Invalid amount.\n");
		return;
	}

	if (
			(who_ref = ematch_me(player_ref, recipient)) == NOTHING
			&& (who_ref = ematch_near(player_ref, recipient)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ who = obj_get(who_ref);

	if (!wizard) {
		if (who.type != TYPE_ENTITY) {
			nd_writef(player_ref, "You can only give to other entities.\n");
			return;
		} else if (who.value + amount > MAX_PENNIES) {
			nd_writef(player_ref, "That player doesn't need that many %s!\n", PENNIES);
			return;
		}
	}

	OBJ player = obj_get(player_ref);

	if (!payfor(player_ref, &player, amount)) {
		nd_writef(player_ref, "You don't have that many %s to give!\n", PENNIES);
		return;
	}

	obj_set(player_ref, &player);
	if (who.type != TYPE_ENTITY) {
		nd_writef(player_ref, "You can't give %s to that!\n", PENNIES);
		return;
	}

	who.value += amount;
	obj_set(who_ref, &who);

	if (amount >= 0) {
		nd_writef(player_ref, "You give %d %s to %s.\n",
				amount, amount == 1 ? PENNY : PENNIES, who.name);

		nd_writef(who_ref, "%s gives you %d %s.\n",
				player.name, amount, amount == 1 ? PENNY : PENNIES);
	} else {
		nd_writef(player_ref, "You take %d %s from %s.\n",
				-amount, amount == -1 ? PENNY : PENNIES, who.name);
		nd_writef(who_ref, "%s takes %d %s from you!\n",
				player.name, -amount, -amount == 1 ? PENNY : PENNIES);
	}
}
