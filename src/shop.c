#include "mdb.h"
#include <stddef.h>
#include "match.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "kill.h"

extern void copyobj(dbref player, dbref old, dbref nu);

static inline dbref
vendor_find(dbref where)
{
	dbref tmp = DBFETCH(where)->contents;

	while (tmp > 0) {
		if (Typeof(tmp) == TYPE_THING && GETSHOP(tmp))
			return tmp;

		tmp = DBFETCH(tmp)->next;
	}

	return NOTHING;
}

void
do_shop(command_t *cmd)
{
	dbref player = cmd->player;
	dbref npc = vendor_find(getloc(player));

	if (npc == NOTHING) {
		notify(player, "No one here wants to buy or sell anything.");
		return;
	}

	notifyf(player, "%s shows you what's for sale.", NAME(npc));
	dbref tmp = DBFETCH(npc)->contents;

	while (tmp > 0) {
		int stock = GETSTACK(tmp);

		if (stock)
			notifyf(player, "%-13s %5dP (%d)",
				NAME(tmp), GETVALUE(tmp), stock);
		else
			notifyf(player, "%-13s %5dP (Inf)",
				NAME(tmp), GETVALUE(tmp));

		tmp = DBFETCH(tmp)->next;
	}
}

void
do_buy(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *amount_s = cmd->argv[2];
	dbref npc = vendor_find(getloc(player));

	if (npc == NOTHING) {
		notify(player, "No one here wants to sell you anything.");
		return;
	}

	int amount = *amount_s ? atoi(amount_s) : 1;
	dbref item = contents_find(cmd, npc, name);

	if (item == NOTHING) {
		notifyf(player, "%s does not sell %s.", NAME(npc), name);
		return;
	}

	int cost = amount * GETVALUE(item);
	int ihave = GETVALUE(player);

	if (ihave < cost) {
		notify(player, "You don't have enough pennies.");
		return;
	}

	int stock = GETSTACK(item);

	if (stock && stock < amount) {
		notifyf(player, "Not enough %s for sale.", NAME(item));
		return;
	}

	SETVALUE(player, ihave - cost);
	SETVALUE(npc, GETVALUE(npc) + cost);

	dbref nu;
	unsigned i;
	if (stock) {
		if (stock == amount) {
			moveto(item, player);
			nu = item;
		} else {
			nu = new_object();
			*DBFETCH(nu) = *DBFETCH(item);
			copyobj(player, item, nu);
			SETSTACK(item, stock - amount);
		}
	} else {
		nu = new_object();
		*DBFETCH(nu) = *DBFETCH(item);
		copyobj(player, item, nu);
	}

	SETCONSUN(nu, GETCONSUM(nu));
	USETSTACK(nu);
	DBDIRTY(nu);

	for (i = 1; i < amount; i++) {
		dbref nu2 = new_object();
		*DBFETCH(nu2) = *DBFETCH(nu);
		copyobj(player, nu, nu2);
	}

	DBDIRTY(npc);
	DBDIRTY(player);
	DBDIRTY(item);

	notifyf(player, "You bought %d %s for %dP.", amount, NAME(item), cost);
}

void
do_sell(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *amount_s = cmd->argv[2];
	dbref npc = vendor_find(getloc(player));

	if (npc == NOTHING) {
		notify(player, "No one here wants to buy you anything.");
		return;
	}

	int amount = *amount_s ? atoi(amount_s) : 1,
	    a = amount;

	dbref item;
	while ((item = contents_find(cmd, player, name)) > 0) {
		int cost = GETVALUE(item);
		int npchas = GETVALUE(npc);

		if (cost > npchas) {
			notifyf(player, "%s can't afford to buy %d %s from you.",
				   NAME(npc), a, NAME(item));
			return;
		}

		// FIXME matched shop item props kept
		dbref nu = contents_find(cmd, npc, NAME(item));

		if (nu == NOTHING) {
			nu = new_object();
			*DBFETCH(nu) = *DBFETCH(item);
			copyobj(npc, item, nu);
			SETSTACK(nu, 1);
		} else
			SETSTACK(nu, GETSTACK(nu) + 1);

		SETVALUE(player, GETVALUE(player) + cost);
		SETVALUE(npc, npchas - cost);

		notifyf(player, "You sold %s for %dP.",
			   NAME(item), cost);

		DBDIRTY(nu);
		recycle(cmd, item);
		a--;
	}

	DBDIRTY(npc);
	DBDIRTY(player);

	if (a) {
		notifyf(player, "You don't have %d %s.", a, name);
		return;
	}
}
