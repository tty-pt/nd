#include "mdb.h"
#include <stddef.h>
#include "match.h"
#include "props.h"
#include "externs.h"
#include "item.h"
#include "kill.h"
#include "web.h"

extern OBJ *object_copy(OBJ *player, OBJ *old);

static inline OBJ *
vendor_find(OBJ *where)
{
	OBJ *tmp = where->contents;

	while (tmp) {
		if (tmp->type == TYPE_ENTITY) {
			ENT *etmp = &tmp->sp.entity;
			if (etmp->flags & EF_SHOP)
				return tmp;
		}

		tmp = tmp->next;
	}

	return NULL;
}

void
do_shop(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	OBJ *here = player->location;
	OBJ *npc = vendor_find(here);
	OBJ *tmp;

	if (!npc) {
		notify(player, "No one here wants to buy or sell anything.");
		return;
	}

	notifyf(player, "%s shows you what's for sale.", npc->name);

        if (!web_look(player, npc))
            return;

	DOLIST(tmp, npc->contents) {
		if (GETINF(tmp))
			notifyf(player, "%-13s %5dP (Inf)",
				tmp->name, tmp->value);
		else
			notifyf(player, "%-13s %5dP",
				tmp->name, tmp->value);
	}
}

void
do_buy(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	OBJ *here = player->location;
	const char *name = cmd->argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		notify(player, "No one here wants to sell you anything.");
		return;
	}

	OBJ *item = ematch_at(player, npc, name);

	if (!item) {
		notifyf(player, "%s does not sell %s.", npc->name, name);
		return;
	}

	int cost = item->value;
	int ihave = player->value;

	if (ihave < cost) {
		notify(player, "You don't have enough pennies.");
		return;
	}

	player->value -= cost;
	npc->value += cost;

        if (GETINF(item)) {
                OBJ *nu = object_copy(player, item);
                USETINF(nu);
                moveto(nu, player);
        } else
                moveto(item, player);

	notifyf(player, "You bought %s for %dP.", item->name, cost);
}

void
do_sell(command_t *cmd)
{
	OBJ *player = object_get(cmd->player),
	    *here = player->location;
	const char *name = cmd->argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		notify(player, "No one here wants to buy you anything.");
		return;
	}

	OBJ *item = ematch_mine(player, name);

	if (!item) {
		notify(player, "You don't have that item.");
		return;
        }

        int cost = item->value;
        int npchas = npc->value;

        if (cost > npchas) {
                notifyf(player, "%s can't afford to buy %s from you.",
                        npc->name, item->name);
                return;
        }

        moveto(item, npc);

        player->value += cost;
        npc->value -= cost;

        notifyf(player, "You sold %s for %dP.",
                item->name, cost);
}
