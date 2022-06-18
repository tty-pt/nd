#include "consumable.h"
#include "io.h"
#include "entity.h"
#include "command.h"
#include "match.h"

#define DRINK_VALUE (1 << 14)
#define FOOD_VALUE(x) (1 << (16 - x->food))

void
do_consume(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *vial;
	CON *cvial;
	int aux;

	if (!*name || !(vial = ematch_mine(player, name))) {
		notify(eplayer, "Consume what?");
		return;
	}

	if (vial->type != TYPE_CONSUMABLE) {
		notify(eplayer, "You can not consume that.");
		return;
	}

	cvial = &vial->sp.consumable;

	if (!cvial->quantity) {
		notifyf(eplayer, "%s is empty.", vial->name);
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
	notify_wts(player, "consume", "consumes", " %s", vial->name);

	if (!cvial->quantity && !cvial->capacity)
		recycle(player, vial);
}

void
do_fill(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *vial_s = cmd->argv[1];
	const char *source_s = cmd->argv[2];
	OBJ *vial = ematch_mine(player, vial_s);
	CON *cvial;
	unsigned m;

	if (!vial) {
		notify(eplayer, "Fill what?");
		return;
	}

	if (vial->type != TYPE_CONSUMABLE) {
		notify(eplayer, "You can not fill that up.");
		return;
	}

	cvial = &vial->sp.consumable;

	if (!(m = cvial->capacity)) {
		notify(eplayer, "You can not fill that up.");
		return;
	}

	if (cvial->quantity) {
		notifyf(eplayer, "%s is not empty.", vial->name);
		return;
	}

	OBJ *source = ematch_near(player, source_s);

	if (!source || source->type != TYPE_CONSUMABLE) {
		notify(eplayer, "Invalid source.");
		return;
	}

	CON *csource = &source->sp.consumable;

	cvial->quantity = m;
	cvial->drink = csource->drink;
	cvial->food = csource->food;

	notify_wts(player, "fill", "fills", " %s from %s",
		vial->name, source->name);
}
