#include "drink.h"

#include <stddef.h>
#include "mob.h"

#include "externs.h"

#define DRINK_VALUE(x) (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - CONSUM(x)->food))

void
do_consume(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	OBJ *vial;
	CON *cvial;
	int aux;

	if (!*name || !(vial = ematch_mine(OBJECT(player), name))) {
		notify(player, "Consume what?");
		return;
	}

	if (vial->type != TYPE_CONSUMABLE) {
		notify(player, "You can not consume that.");
		return;
	}

	cvial = &vial->sp.consumable;

	if (!cvial->quantity) {
		notifyf(player, "%s is empty.", vial->name);
		return;
	}

	if (cvial->drink) {
		aux = ENTITY(player)->thirst - DRINK_VALUE(REF(vial));
		ENTITY(player)->thirst = aux < 0 ? 0 : aux;
	}

	if (cvial->food) {
		aux = ENTITY(player)->hunger - FOOD_VALUE(REF(vial));
		ENTITY(player)->hunger = aux < 0 ? 0 : aux;
	}

	cvial->quantity--;
	notify_wts(player, "consume", "consumes", " %s", vial->name);

	if (!cvial->quantity && !cvial->capacity)
		recycle(player, REF(vial));
}

void
do_fill(command_t *cmd)
{
	dbref player = cmd->player;
	const char *vial_s = cmd->argv[1];
	const char *source_s = cmd->argv[2];
	OBJ *vial = ematch_mine(OBJECT(player), vial_s);
	CON *cvial;
	unsigned m;

	if (!vial) {
		notify(player, "Fill what?");
		return;
	}

	if (vial->type != TYPE_CONSUMABLE) {
		notify(player, "You can not fill that up.");
		return;
	}

	cvial = &vial->sp.consumable;

	if (!(m = cvial->capacity)) {
		notify(player, "You can not fill that up.");
		return;
	}

	if (cvial->quantity) {
		notifyf(player, "%s is not empty.", vial->name);
		return;
	}

	OBJ *source = ematch_near(OBJECT(player), source_s);

	if (!source || source->type != TYPE_CONSUMABLE) {
		notify(player, "Invalid source.");
		return;
	}

	CON *csource = &source->sp.consumable;

	cvial->quantity = m;
	cvial->drink = csource->drink;
	cvial->food = csource->food;

	notify_wts(player, "fill", "fills", " %s from %s",
		vial->name, source->name);
}
