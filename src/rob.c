#include "io.h"
#include "entity.h"
#include "params.h"
#include "defaults.h"
#include "match.h"
#include "command.h"

void
do_give(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *recipient = cmd->argv[1];
	int amount = atoi(cmd->argv[2]);
	OBJ *who;

	if (amount < 0 && !(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Invalid amount.");
		return;
	}

	if (
			!(who = ematch_me(player, recipient))
			&& !(who = ematch_near(player, recipient))
	   )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	ENT *ewho = &who->sp.entity;

	if (!(eplayer->flags & EF_WIZARD)) {
		if (who->type != TYPE_ENTITY) {
			notify(eplayer, "You can only give to other entities.");
			return;
		} else if (who->value + amount > MAX_PENNIES) {
			notifyf(eplayer, "That player doesn't need that many %s!", PENNIES);
			return;
		}
	}

	if (!payfor(player, amount)) {
		notifyf(eplayer, "You don't have that many %s to give!", PENNIES);
		return;
	}

	if (who->type != TYPE_ENTITY) {
		notifyf(eplayer, "You can't give %s to that!", PENNIES);
		return;
	}

	who->value += amount;

	if (amount >= 0) {
		notifyf(eplayer, "You give %d %s to %s.",
				amount, amount == 1 ? PENNY : PENNIES, who->name);

		notifyf(ewho, "%s gives you %d %s.",
				player->name, amount, amount == 1 ? PENNY : PENNIES);
	} else {
		notifyf(eplayer, "You take %d %s from %s.",
				-amount, amount == -1 ? PENNY : PENNIES, who->name);
		notifyf(ewho, "%s takes %d %s from you!",
				player->name, -amount, -amount == 1 ? PENNY : PENNIES);
	}
}
