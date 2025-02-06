#include "uapi/io.h"

#include <stdlib.h>

#include "uapi/entity.h"
#include "uapi/match.h"
#include "uapi/wts.h"

void
do_give(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), who_ref;
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

	OBJ who;
	lhash_get(obj_hd, &who, who_ref);

	if (!wizard && who.type != TYPE_ENTITY) {
		nd_writef(player_ref, "You can only give to other entities.\n");
		return;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);

	if (!payfor(player_ref, &player, amount)) {
		nd_writef(player_ref, "You don't have that many shekels to give!\n");
		return;
	}

	lhash_put(obj_hd, player_ref, &player);
	if (who.type != TYPE_ENTITY) {
		nd_writef(player_ref, "You can't give shekels to that!\n");
		return;
	}

	who.value += amount;
	lhash_put(obj_hd, who_ref, &who);

	char *unit = wts_cond("shekel", amount);

	if (amount >= 0) {
		nd_writef(player_ref, "You give %d %s to %s.\n", amount, unit, who.name);

		nd_writef(who_ref, "%s gives you %d %s.\n", player.name, amount, unit);
	} else {
		nd_writef(player_ref, "You take %d %s from %s.\n", -amount, unit, who.name);
		nd_writef(who_ref, "%s takes %d %s from you!\n", player.name, -amount, unit);
	}
}
