#include <ndc.h>
#include "io.h"
#include "entity.h"
#include <stddef.h>
#include "match.h"
#include "object.h"

static inline dbref
vendor_find(dbref where_ref)
{
	struct hash_cursor c = contents_iter(where_ref);
	dbref tmp_ref;

	while ((tmp_ref = contents_next(&c)) != NOTHING)
		if (obj_get(tmp_ref).type == TYPE_ENTITY)
			if (ent_get(tmp_ref).flags & EF_SHOP) {
				hash_fin(&c);
				return tmp_ref;
			}

	return NOTHING;
}

void
do_shop(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd),
	      npc_ref = vendor_find(obj_get(player_ref).location),
	      tmp_ref;

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to buy or sell anything.\n");
		return;
	}

	nd_writef(player_ref, "%s shows you what's for sale.\n", obj_get(npc_ref).name);
        mcp_look(player_ref, npc_ref);

	struct hash_cursor c = contents_iter(npc_ref);
	while ((tmp_ref = contents_next(&c)) != NOTHING) {
		OBJ tmp = obj_get(tmp_ref);
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
	dbref player_ref = FD_PLAYER(fd);
	OBJ player = obj_get(player_ref);
	char *name = argv[1];
	dbref npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to sell you anything.\n");
		return;
	}

	OBJ npc = obj_get(npc_ref);
	dbref item_ref = ematch_at(player_ref, npc_ref, name);

	if (item_ref == NOTHING) {
		nd_writef(player_ref, "%s does not sell %s.\n", npc.name, name);
		return;
	}

	OBJ item = obj_get(item_ref);
	int cost = item.value;
	int ihave = player.value;

	if (ihave < cost) {
		nd_writef(player_ref, "You don't have enough pennies.\n");
		return;
	}

	player.value -= cost;
	npc.value += cost;
	obj_set(npc_ref, &npc);
	obj_set(player_ref, &player);

        if (item.flags & OF_INF) {
		OBJ nu;
		dbref nu_ref = object_copy(&nu, item_ref);
		nu.flags &= ~OF_INF;
		obj_set(nu_ref, &nu);
                object_move(nu_ref, player_ref);
        } else
                object_move(item_ref, player_ref);

	nd_writef(player_ref, "You bought %s for %dP.\n", item.name, cost);
}

void
do_sell(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	OBJ player = obj_get(player_ref);
	dbref npc_ref = vendor_find(player.location);

	if (npc_ref == NOTHING) {
		nd_writef(player_ref, "No one here wants to buy you anything.\n");
		return;
	}

	dbref item_ref = ematch_mine(player_ref, argv[1]);

	if (item_ref == NOTHING) {
		nd_writef(player_ref, "You don't have that item.\n");
		return;
        }

	OBJ npc = obj_get(npc_ref);
	OBJ item = obj_get(item_ref);
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
	obj_set(player_ref, &player);
	obj_set(npc_ref, &npc);

        nd_writef(player_ref, "You sold %s for %dP.\n",
                item.name, cost);
}
