#include <ndc.h>
#include "io.h"
#include "entity.h"
#include <stddef.h>
#include "match.h"
#include "object.h"

static inline OBJ *
vendor_find(OBJ *where)
{
	struct hash_cursor c = contents_iter(object_ref(where));

	OBJ *tmp;

	while ((tmp = contents_next(&c)))
		if (tmp->type == TYPE_ENTITY) {
			ENT *etmp = &tmp->sp.entity;
			if (etmp->flags & EF_SHOP)
				return tmp;
		}

	return NULL;
}

void
do_shop(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	OBJ *here = object_get(player->location);
	OBJ *npc = vendor_find(here);
	OBJ *tmp;

	if (!npc) {
		nd_writef(player, "No one here wants to buy or sell anything.\n");
		return;
	}

	nd_writef(player, "%s shows you what's for sale.\n", npc->name);

        mcp_look(player, npc);

	struct hash_cursor c = contents_iter(object_ref(npc));
	while ((tmp = contents_next(&c))) {
		if (tmp->flags & OF_INF)
			nd_writef(player, "%-13s %5dP (Inf)",
				tmp->name, tmp->value);
		else
			nd_writef(player, "%-13s %5dP",
				tmp->name, tmp->value);
	}
}

void
do_buy(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	OBJ *here = object_get(player->location);
	char *name = argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		nd_writef(player, "No one here wants to sell you anything.\n");
		return;
	}

	OBJ *item = ematch_at(player, npc, name);

	if (!item) {
		nd_writef(player, "%s does not sell %s.\n", npc->name, name);
		return;
	}

	int cost = item->value;
	int ihave = player->value;

	if (ihave < cost) {
		nd_writef(player, "You don't have enough pennies.\n");
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

	nd_writef(player, "You bought %s for %dP.\n", item->name, cost);
}

void
do_sell(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd),
	    *here = object_get(player->location);
	char *name = argv[1];
	OBJ *npc = vendor_find(here);

	if (!npc) {
		nd_writef(player, "No one here wants to buy you anything.\n");
		return;
	}

	OBJ *item = ematch_mine(player, name);

	if (!item) {
		nd_writef(player, "You don't have that item.\n");
		return;
        }

        int cost = item->value;
        int npchas = npc->value;

        if (cost > npchas) {
                nd_writef(player, "%s can't afford to buy %s from you.\n",
                        npc->name, item->name);
                return;
        }

        object_move(item, npc);

        player->value += cost;
        npc->value -= cost;

        nd_writef(player, "You sold %s for %dP.\n",
                item->name, cost);
}
