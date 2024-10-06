#include "uapi/io.h"

#include "mcp.h"
#include "uapi/entity.h"
#include "uapi/match.h"
#include "uapi/object.h"

static inline unsigned
vendor_find(unsigned where_ref)
{
	struct hash_cursor c = fhash_iter(contents_hd, where_ref);
	unsigned tmp_ref;

	while (ahash_next(&tmp_ref, &c)) {
		OBJ tmp;
		lhash_get(obj_hd, &tmp, tmp_ref);
		if (tmp.type != TYPE_ENTITY)
			continue;
		if (ent_get(tmp_ref).flags & EF_SHOP) {
			hash_fin(&c);
			return tmp_ref;
		}
	}

	return NOTHING;
}

void
do_shop(int fd, int argc, char *argv[])
{
	OBJ player, npc;
	unsigned player_ref = fd_player(fd),
		 npc_ref, tmp_ref;

	lhash_get(obj_hd, &player, player_ref);
	npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to buy or sell anything.\n");
		return;
	}

	lhash_get(obj_hd, &npc, npc_ref);
	nd_writef(player_ref, "%s shows you what's for sale.\n", npc.name);
        mcp_look(player_ref, npc_ref);

	struct hash_cursor c = fhash_iter(obj_hd, npc_ref);
	while (ahash_next(&tmp_ref, &c)) {
		OBJ tmp;
		lhash_get(obj_hd, &tmp, tmp_ref);
		if (tmp.flags & OF_INF)
			nd_writef(player_ref, "%-13s %5dP (Inf)",
				tmp.name, tmp.value);
		else
			nd_writef(player_ref, "%-13s %5dP",
				tmp.name, tmp.value);
	}
}

void
do_buy(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	char *name = argv[1];
	unsigned npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to sell you anything.\n");
		return;
	}

	OBJ npc;
	lhash_get(obj_hd, &npc, npc_ref);
	unsigned item_ref = ematch_at(player_ref, npc_ref, name);

	if (item_ref == NOTHING) {
		nd_writef(player_ref, "%s does not sell %s.\n", npc.name, name);
		return;
	}

	OBJ item;
	lhash_get(obj_hd, &item, item_ref);
	int cost = item.value;
	int ihave = player.value;

	if (ihave < cost) {
		nd_writef(player_ref, "You don't have enough pennies.\n");
		return;
	}

	player.value -= cost;
	npc.value += cost;
	lhash_put(obj_hd, npc_ref, &npc);
	lhash_put(obj_hd, player_ref, &player);

        if (item.flags & OF_INF) {
		OBJ nu;
		unsigned nu_ref = object_copy(&nu, item_ref);
		nu.flags &= ~OF_INF;
		lhash_put(obj_hd, nu_ref, &nu);
                object_move(nu_ref, player_ref);
        } else
                object_move(item_ref, player_ref);

	nd_writef(player_ref, "You bought %s for %dP.\n", item.name, cost);
}

void
do_sell(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
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

	lhash_get(obj_hd, &npc, npc_ref);
	lhash_get(obj_hd, &item, item_ref);
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
	lhash_put(obj_hd, player_ref, &player);
	lhash_put(obj_hd, npc_ref, &npc);

        nd_writef(player_ref, "You sold %s for %dP.\n", item.name, cost);
}
