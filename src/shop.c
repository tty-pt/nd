#include "io.h"
#include "entity.h"
#include "mdb.h"
#include <stddef.h>
#include "match.h"
#include "props.h"
#include "externs.h"

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
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	OBJ *here = player->location;
	OBJ *npc = vendor_find(here);
	OBJ *tmp;

	if (!npc) {
		notify(eplayer, "No one here wants to buy or sell anything.");
		return;
	}

	notifyf(eplayer, "%s shows you what's for sale.", npc->name);

        if (!mcp_look(player, npc))
            return;

	FOR_LIST(tmp, npc->contents) {
		if (tmp->flags & OF_INF)
			notifyf(eplayer, "%-13s %5dP (Inf)",
				tmp->name, tmp->value);
		else
			notifyf(eplayer, "%-13s %5dP",
				tmp->name, tmp->value);
	}
}

void
do_buy(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	OBJ *here = player->location;
	const char *name = cmd->argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		notify(eplayer, "No one here wants to sell you anything.");
		return;
	}

	OBJ *item = ematch_at(player, npc, name);

	if (!item) {
		notifyf(eplayer, "%s does not sell %s.", npc->name, name);
		return;
	}

	int cost = item->value;
	int ihave = player->value;

	if (ihave < cost) {
		notify(eplayer, "You don't have enough pennies.");
		return;
	}

	player->value -= cost;
	npc->value += cost;

        if (item->flags & OF_INF) {
                OBJ *nu = object_copy(player, item);
		nu->flags &= ~OF_INF;
                object_move(nu, player);
        } else
                object_move(item, player);

	notifyf(eplayer, "You bought %s for %dP.", item->name, cost);
}

void
do_sell(command_t *cmd)
{
	OBJ *player = cmd->player,
	    *here = player->location;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		notify(eplayer, "No one here wants to buy you anything.");
		return;
	}

	OBJ *item = ematch_mine(player, name);

	if (!item) {
		notify(eplayer, "You don't have that item.");
		return;
        }

        int cost = item->value;
        int npchas = npc->value;

        if (cost > npchas) {
                notifyf(eplayer, "%s can't afford to buy %s from you.",
                        npc->name, item->name);
                return;
        }

        object_move(item, npc);

        player->value += cost;
        npc->value -= cost;

        notifyf(eplayer, "You sold %s for %dP.",
                item->name, cost);
}
