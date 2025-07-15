#include <nd/nd.h>

#include "./include/drink.h"

#define DRINK_VALUE (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - x->food))

unsigned act_fill, act_drink, type_consumable, vtf_pond;

void
do_consume(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), vial_ref;
	ENT eplayer = ent_get(player_ref);
	char *name = argv[1];
	int aux;

	if (!*name || (vial_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ vial;
	nd_get(HD_OBJ, &vial, &vial_ref);

	if (vial.type != type_consumable) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	CON *cvial = (CON *) &vial.sp.raw;

	if (!cvial->quantity) {
		nd_writef(player_ref, "%s is empty.\n", vial.name);
		return;
	}

	if (cvial->drink) {
		aux = eplayer.huth[HUTH_THIRST] - DRINK_VALUE;
		eplayer.huth[HUTH_THIRST] = aux < 0 ? 0 : aux;
	}

	if (cvial->food) {
		aux = eplayer.huth[HUTH_HUNGER] - FOOD_VALUE(cvial);
		eplayer.huth[HUTH_HUNGER] = aux < 0 ? 0 : aux;
	}

	cvial->quantity--;
	OBJ player;
	nd_get(HD_OBJ, &player, &player_ref);
	nd_owritef(player_ref, "%s consumes %s\n", player.name, vial.name);

	SIC_CALL(NULL, sic_consume, player_ref, vial_ref);

	if (!cvial->quantity && !cvial->capacity)
		object_move(vial_ref, NOTHING);
	else
		nd_put(HD_OBJ, &vial_ref, &vial);

	ent_set(player_ref, &eplayer);
}

void
do_fill(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd),
		 vial_ref = ematch_mine(player_ref, argv[1]),
		 source_ref = ematch_near(player_ref, argv[2]);

	if (vial_ref == NOTHING || source_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ vial;
	nd_get(HD_OBJ, &vial, &vial_ref);
	CON *cvial = (CON *) &vial.sp.raw;

	if (vial.type != type_consumable || !cvial->capacity)
		goto error;

	OBJ source;
	nd_get(HD_OBJ, &source, &source_ref);

	if (source.type != type_consumable)
		goto error;

	CON *csource = (CON *) &source.sp.raw;
	cvial->quantity = cvial->capacity;
	cvial->drink = csource->drink;
	cvial->food = csource->food;
	nd_put(HD_OBJ, &vial_ref, &vial);

	OBJ player;
	nd_get(HD_OBJ, &player, &player_ref);
	nd_owritef(player_ref, "%s fills %s from %s\n", player.name, vial.name, source.name);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

struct icon sic_icon(struct icon i,
		unsigned ref __attribute__((unused)),
		OBJ obj)
{
	CON *cwhat;

	if (obj.type != type_consumable)
		return i;

	cwhat = (CON *) &obj.sp.raw;

	if (cwhat->drink) {
		i.actions |= act_fill;
		i.pi.fg = BLUE;
		i.ch = '~';
	} else {
		i.pi.fg = RED;
		i.ch = 'o';
	}

	i.actions |= act_drink;
	return i;
}

OBJ sic_add(unsigned ref __attribute__((unused)),
		OBJ obj,
		unsigned skel_id __attribute__((unused)),
		SKEL skel,
		unsigned where_id __attribute__((unused)))
{
	CON *cnu = (CON *) &obj.sp.raw;

	obj.type = type_consumable;
	cnu->food = skel.sp.consumable.food;
	cnu->drink = skel.sp.consumable.drink;
	return obj;
}

unsigned short sic_view_flags(unsigned short flags,
		unsigned ref __attribute__((unused)),
		OBJ obj)
{
	if (obj.type != type_consumable)
		return flags;

	return flags | vtf_pond;
}

void mod_open(void *arg __attribute__((unused))) {
	vtf_pond = vtf_register('~', BLUE, BOLD);
	act_fill = action_register("fill", "💧");
	act_drink = action_register("drink", "🧪");

	type_consumable = nd_put(HD_TYPE, NULL, "drink");

	nd_register("consume", do_consume, 0);
	nd_register("fill", do_fill, 0);

	/* sic_reg("sic_add"); */
	/* sic_reg("sic_view_flags"); */
	/* sic_reg("sic_icon"); */

	sic_areg("sic_consume");
}

void mod_install(void *arg) {
	mod_open(arg);
}
