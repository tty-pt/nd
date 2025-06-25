#include <nd/nd.h>
#include "mcp.h"

static inline unsigned
vendor_find(unsigned where_ref)
{
	nd_cur_t c = nd_iter(HD_CONTENTS, &where_ref);
	unsigned tmp_ref;

	while (nd_next(&where_ref, &tmp_ref, &c)) {
		OBJ tmp;
		nd_get(HD_OBJ, &tmp, &tmp_ref);
		if (tmp.type != TYPE_ENTITY)
			continue;
		if (ent_get(tmp_ref).flags & EF_SHOP) {
			nd_fin(&c);
			return tmp_ref;
		}
	}

	return NOTHING;
}

void
do_shop(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	OBJ player, npc;
	unsigned player_ref = fd_player(fd),
		 npc_ref, tmp_ref;

	nd_get(HD_OBJ, &player, &player_ref);
	npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to buy or sell anything.\n");
		return;
	}

	nd_get(HD_OBJ, &npc, &npc_ref);
	nd_writef(player_ref, "%s shows you what's for sale.\n", npc.name);
	look_at(player_ref, npc_ref);

	nd_cur_t c = nd_iter(HD_OBJ, &npc_ref);
	while (nd_next(&npc_ref, &tmp_ref, &c)) {
		OBJ tmp;
		nd_get(HD_OBJ, &tmp, &tmp_ref);
		if (tmp.flags & OF_INF)
			nd_writef(player_ref, "%-13s %5dP (Inf)",
					tmp.name, tmp.value);
		else
			nd_writef(player_ref, "%-13s %5dP",
					tmp.name, tmp.value);
	}
}

void
do_buy(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player;
	nd_get(HD_OBJ, &player, &player_ref);
	char *name = argv[1];
	unsigned npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to sell you anything.\n");
		return;
	}

	OBJ npc;
	nd_get(HD_OBJ, &npc, &npc_ref);
	unsigned item_ref = ematch_at(player_ref, npc_ref, name);

	if (item_ref == NOTHING) {
		nd_writef(player_ref, "%s does not sell %s.\n", npc.name, name);
		return;
	}

	OBJ item;
	nd_get(HD_OBJ, &item, &item_ref);
	int cost = item.value;
	int ihave = player.value;

	if (ihave < cost) {
		nd_writef(player_ref, "You don't have enough pennies.\n");
		return;
	}

	player.value -= cost;
	npc.value += cost;
	nd_put(HD_OBJ, &npc_ref, &npc);
	nd_put(HD_OBJ, &player_ref, &player);

	if (item.flags & OF_INF) {
		OBJ nu;
		unsigned nu_ref = object_copy(&nu, item_ref);
		nu.flags &= ~OF_INF;
		nd_put(HD_OBJ, &nu_ref, &nu);
		object_move(nu_ref, player_ref);
	} else
		object_move(item_ref, player_ref);

	nd_writef(player_ref, "You bought %s for %dP.\n", item.name, cost);
}

void
do_sell(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player;
	nd_get(HD_OBJ, &player, &player_ref);
	unsigned npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to buy you anything.\n");
		return;
	}

	unsigned item_ref = ematch_mine(player_ref, argv[1]);

	if (item_ref == NOTHING) {
		nd_writef(player_ref, "You don't have that item.\n");
		return;
	}

	OBJ npc, item;

	nd_get(HD_OBJ, &npc, &npc_ref);
	nd_get(HD_OBJ, &item, &item_ref);
	int cost = item.value;
	int npchas = npc.value;

	if (cost > npchas) {
		nd_writef(player_ref, "%s can't afford to buy %s from you.\n",
				npc.name, item.name);
		return;
	}

	object_move(item_ref, npc_ref);

	player.value += cost;
	npc.value -= cost;
	nd_put(HD_OBJ, &player_ref, &player);
	nd_put(HD_OBJ, &npc_ref, &npc);

	nd_writef(player_ref, "You sold %s for %dP.\n", item.name, cost);
}

void mod_install(void *arg __attribute__((unused))) {
	nd_register("shop", do_shop, 0);
	nd_register("buy", do_buy, 0);
	nd_register("sell", do_sell, 0);
}

void mod_open(void *arg __attribute__((unused))) {
	mod_install(arg);
}
