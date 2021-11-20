#include "mdb.h"
#include <stddef.h>
#include "match.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "kill.h"
#include "web.h"

extern dbref object_copy(dbref player, dbref old);

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

        if (!web_look(cmd, npc, GETDESC(npc)))
            return;

	dbref tmp = DBFETCH(npc)->contents;

	while (tmp > 0) {
		if (GETINF(tmp))
			notifyf(player, "%-13s %5dP (Inf)",
				NAME(tmp), GETVALUE(tmp));
		else
			notifyf(player, "%-13s %5dP",
				NAME(tmp), GETVALUE(tmp));

		tmp = DBFETCH(tmp)->next;
	}
}

void
do_buy(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref npc = vendor_find(getloc(player));

	if (npc == NOTHING) {
		notify(player, "No one here wants to sell you anything.");
		return;
	}

	dbref item = contents_find(cmd, npc, name);

	if (item == NOTHING) {
		notifyf(player, "%s does not sell %s.", NAME(npc), name);
		return;
	}

	int cost = GETVALUE(item);
	int ihave = GETVALUE(player);

	if (ihave < cost) {
		notify(player, "You don't have enough pennies.");
		return;
	}

	SETVALUE(player, ihave - cost);
	SETVALUE(npc, GETVALUE(npc) + cost);

        if (GETINF(item)) {
                dbref nu = object_copy(player, item);
                USETINF(nu);
                moveto(nu, player);
        } else
                moveto(item, player);

	notifyf(player, "You bought %s for %dP.", NAME(item), cost);
}

void
do_sell(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref npc = vendor_find(getloc(player));

	if (npc == NOTHING) {
		notify(player, "No one here wants to buy you anything.");
		return;
	}

	dbref item = contents_find(cmd, player, name);

        if (item == NOTHING) {
		notify(player, "You don't have that item.");
		return;
        }

        int cost = GETVALUE(item);
        int npchas = GETVALUE(npc);

        if (cost > npchas) {
                notifyf(player, "%s can't afford to buy %s from you.",
                        NAME(npc), NAME(item));
                return;
        }

        moveto(item, npc);

        SETVALUE(player, GETVALUE(player) + cost);
        SETVALUE(npc, npchas - cost);

        notifyf(player, "You sold %s for %dP.",
                NAME(item), cost);
}
