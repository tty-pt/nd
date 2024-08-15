#include <ndc.h>
#include "consumable.h"
#include "io.h"
#include "entity.h"
#include "match.h"

#define DRINK_VALUE (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - x->food))

void
do_consume(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd), vial_ref;
	ENT eplayer = ent_get(player_ref);
	char *name = argv[1];
	int aux;

	if (!*name || (vial_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, "Consume what?\n");
		return;
	}

	OBJ vial = obj_get(vial_ref);

	if (vial.type != TYPE_CONSUMABLE) {
		nd_writef(player_ref, "You can not consume that.\n");
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
	notify_wts(player_ref, "consume", "consumes", " %s\n", vial.name);

	if (!cvial->quantity && !cvial->capacity)
		obj_del(vial_ref);
	else
		obj_set(vial_ref, &vial);

	ent_set(player_ref, &eplayer);
}

void
do_fill(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd),
	      vial_ref = ematch_mine(player_ref, argv[1]);;

	if (vial_ref == NOTHING) {
		nd_writef(player_ref, "Fill what?\n");
		return;
	}

	OBJ vial = obj_get(vial_ref);

	if (vial.type != TYPE_CONSUMABLE) {
		nd_writef(player_ref, "You can not fill that up.\n");
		return;
	}

	CON *cvial = &vial.sp.consumable;

	if (!cvial->capacity) {
		nd_writef(player_ref, "You can not fill that up.\n");
		return;
	}

	if (cvial->quantity) {
		nd_writef(player_ref, "%s is not empty.\n", vial.name);
		return;
	}

	dbref source_ref = ematch_near(player_ref, argv[2]);

	if (source_ref == NOTHING) {
		nd_writef(player_ref, "Invalid source.\n");
		return;
	}

	OBJ source = obj_get(source_ref);

	if (source.type != TYPE_CONSUMABLE) {
		nd_writef(player_ref, "Invalid source.\n");
		return;
	}

	CON *csource = &source.sp.consumable;
	cvial->quantity = cvial->capacity;
	cvial->drink = csource->drink;
	cvial->food = csource->food;
	obj_set(vial_ref, &vial);

	notify_wts(player_ref, "fill", "fills", " %s from %s\n",
		vial.name, source.name);
}
