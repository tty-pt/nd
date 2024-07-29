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
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *recipient = argv[1];
	int amount = atoi(argv[2]);
	OBJ *who;

	if (amount < 0 && !(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "Invalid amount.\n");
		return;
	}

	if (
			!(who = ematch_me(player, recipient))
			&& !(who = ematch_near(player, recipient))
	   )
	{
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	if (!(eplayer->flags & EF_WIZARD)) {
		if (who->type != TYPE_ENTITY) {
			nd_writef(player, "You can only give to other entities.\n");
			return;
		} else if (who->value + amount > MAX_PENNIES) {
			nd_writef(player, "That player doesn't need that many %s!\n", PENNIES);
			return;
		}
	}

	if (!payfor(player, amount)) {
		nd_writef(player, "You don't have that many %s to give!\n", PENNIES);
		return;
	}

	if (who->type != TYPE_ENTITY) {
		nd_writef(player, "You can't give %s to that!\n", PENNIES);
		return;
	}

	who->value += amount;

	if (amount >= 0) {
		nd_writef(player, "You give %d %s to %s.\n",
				amount, amount == 1 ? PENNY : PENNIES, who->name);

		nd_writef(who, "%s gives you %d %s.\n",
				player->name, amount, amount == 1 ? PENNY : PENNIES);
	} else {
		nd_writef(player, "You take %d %s from %s.\n",
				-amount, amount == -1 ? PENNY : PENNIES, who->name);
		nd_writef(who, "%s takes %d %s from you!\n",
				player->name, -amount, -amount == 1 ? PENNY : PENNIES);
	}
}
