#include <ndc.h>
#include "io.h"
#include "entity.h"
#include <stddef.h>
#include "match.h"

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
do_shop(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	OBJ *here = player->location;
	OBJ *npc = vendor_find(here);
	OBJ *tmp;

	if (!npc) {
		ndc_writef(fd, "No one here wants to buy or sell anything.\n");
		return;
	}

	ndc_writef(fd, "%s shows you what's for sale.\n", npc->name);

        if (!mcp_look(player, npc))
            return;

	FOR_LIST(tmp, npc->contents) {
		if (tmp->flags & OF_INF)
			ndc_writef(fd, "%-13s %5dP (Inf)",
				tmp->name, tmp->value);
		else
			ndc_writef(fd, "%-13s %5dP",
				tmp->name, tmp->value);
	}
}

void
do_buy(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	OBJ *here = player->location;
	char *name = argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		ndc_writef(fd, "No one here wants to sell you anything.\n");
		return;
	}

	OBJ *item = ematch_at(player, npc, name);

	if (!item) {
		ndc_writef(fd, "%s does not sell %s.\n", npc->name, name);
		return;
	}

	int cost = item->value;
	int ihave = player->value;

	if (ihave < cost) {
		ndc_writef(fd, "You don't have enough pennies.\n");
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

	ndc_writef(fd, "You bought %s for %dP.\n", item->name, cost);
}

void
do_sell(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd),
	    *here = player->location;
	char *name = argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		ndc_writef(fd, "No one here wants to buy you anything.\n");
		return;
	}

	OBJ *item = ematch_mine(player, name);

	if (!item) {
		ndc_writef(fd, "You don't have that item.\n");
		return;
        }

        int cost = item->value;
        int npchas = npc->value;

        if (cost > npchas) {
                ndc_writef(fd, "%s can't afford to buy %s from you.\n",
                        npc->name, item->name);
                return;
        }

        object_move(item, npc);

        player->value += cost;
        npc->value -= cost;

        ndc_writef(fd, "You sold %s for %dP.\n",
                item->name, cost);
}
