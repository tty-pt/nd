#include "uapi/io.h"
#include "uapi/entity.h"
#include "uapi/match.h"
#include <qdb.h>

#define DRINK_VALUE (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - x->food))

void
do_consume(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), vial_ref;
	ENT eplayer = ent_get(player_ref);
	char *name = argv[1];
	int aux;

	if (!*name || (vial_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ vial;
	qdb_get(obj_hd, &vial, &vial_ref);

	if (vial.type != TYPE_CONSUMABLE) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	CON *cvial = &vial.sp.consumable;

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
	qdb_get(obj_hd, &player, &player_ref);
	nd_owritef(player_ref, "%s consumes %s\n", player.name, vial.name);

	if (!cvial->quantity && !cvial->capacity)
		object_move(vial_ref, NOTHING);
	else
		qdb_put(obj_hd, &vial_ref, &vial);

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
	qdb_get(obj_hd, &vial, &vial_ref);
	CON *cvial = &vial.sp.consumable;

	if (vial.type != TYPE_CONSUMABLE || !cvial->capacity)
		goto error;

	OBJ source;
	qdb_get(obj_hd, &source, &source_ref);

	if (source.type != TYPE_CONSUMABLE)
		goto error;

	CON *csource = &source.sp.consumable;
	cvial->quantity = cvial->capacity;
	cvial->drink = csource->drink;
	cvial->food = csource->food;
	qdb_put(obj_hd, &vial_ref, &vial);

	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	nd_owritef(player_ref, "%s fills %s from %s\n", player.name, vial.name, source.name);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}
