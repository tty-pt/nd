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
	dbref tmp = OBJECT(where)->contents;

	while (tmp > 0) {
		if (OBJECT(tmp)->type == TYPE_ENTITY && (ENTITY(tmp)->flags & EF_SHOP))
			return tmp;

		tmp = OBJECT(tmp)->next;
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

	notifyf(player, "%s shows you what's for sale.", OBJECT(npc)->name);

        if (!web_look(player, npc))
            return;

	dbref tmp = OBJECT(npc)->contents;

	while (tmp > 0) {
		if (GETINF(tmp))
			notifyf(player, "%-13s %5dP (Inf)",
				OBJECT(tmp)->name, OBJECT(tmp)->value);
		else
			notifyf(player, "%-13s %5dP",
				OBJECT(tmp)->name, OBJECT(tmp)->value);

		tmp = OBJECT(tmp)->next;
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

	OBJ *item = ematch_at(OBJECT(player), OBJECT(npc), name);

	if (!item) {
		notifyf(player, "%s does not sell %s.", OBJECT(npc)->name, name);
		return;
	}

	int cost = item->value;
	int ihave = OBJECT(player)->value;

	if (ihave < cost) {
		notify(player, "You don't have enough pennies.");
		return;
	}

	OBJECT(player)->value -= cost;
	OBJECT(npc)->value += cost;

        if (GETINF(REF(item))) {
                dbref nu = object_copy(player, REF(item));
                USETINF(nu);
                moveto(nu, player);
        } else
                moveto(REF(item), player);

	notifyf(player, "You bought %s for %dP.", item->name, cost);
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

	OBJ *item = ematch_mine(OBJECT(player), name);

	if (!item) {
		notify(player, "You don't have that item.");
		return;
        }

        int cost = item->value;
        int npchas = OBJECT(npc)->value;

        if (cost > npchas) {
                notifyf(player, "%s can't afford to buy %s from you.",
                        OBJECT(npc)->name, item->name);
                return;
        }

        moveto(REF(item), npc);

        OBJECT(player)->value += cost;
        OBJECT(npc)->value -= cost;

        notifyf(player, "You sold %s for %dP.",
                item->name, cost);
}
