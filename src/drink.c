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
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	OBJ *vial;
	CON *cvial;
	int aux;

	if (!*name || !(vial = ematch_mine(player, name))) {
		nd_writef(player, "Consume what?\n");
		return;
	}

	if (vial->type != TYPE_CONSUMABLE) {
		nd_writef(player, "You can not consume that.\n");
		return;
	}

	cvial = &vial->sp.consumable;

	if (!cvial->quantity) {
		nd_writef(player, "%s is empty.\n", vial->name);
		return;
	}

	if (cvial->drink) {
		aux = eplayer->thirst - DRINK_VALUE;
		eplayer->thirst = aux < 0 ? 0 : aux;
	}

	if (cvial->food) {
		aux = eplayer->hunger - FOOD_VALUE(cvial);
		eplayer->hunger = aux < 0 ? 0 : aux;
	}

	cvial->quantity--;
	notify_wts(player, "consume", "consumes", " %s\n", vial->name);

	if (!cvial->quantity && !cvial->capacity)
		recycle(player, vial);
}

void
do_fill(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *vial_s = argv[1];
	char *source_s = argv[2];
	OBJ *vial = ematch_mine(player, vial_s);
	CON *cvial;
	unsigned m;

	if (!vial) {
		nd_writef(player, "Fill what?\n");
		return;
	}

	if (vial->type != TYPE_CONSUMABLE) {
		nd_writef(player, "You can not fill that up.\n");
		return;
	}

	cvial = &vial->sp.consumable;

	if (!(m = cvial->capacity)) {
		nd_writef(player, "You can not fill that up.\n");
		return;
	}

	if (cvial->quantity) {
		nd_writef(player, "%s is not empty.\n", vial->name);
		return;
	}

	OBJ *source = ematch_near(player, source_s);

	if (!source || source->type != TYPE_CONSUMABLE) {
		nd_writef(player, "Invalid source.\n");
		return;
	}

	CON *csource = &source->sp.consumable;

	cvial->quantity = m;
	cvial->drink = csource->drink;
	cvial->food = csource->food;

	notify_wts(player, "fill", "fills", " %s from %s\n",
		vial->name, source->name);
}
