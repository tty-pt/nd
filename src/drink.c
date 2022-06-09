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
	dbref vial;
	int aux;

	if (!*name || (vial = ematch_mine(player, name)) == NOTHING) {
		notify(player, "Consume what?");
		return;
	}

	if (Typeof(vial) != TYPE_CONSUMABLE) {
		notify(player, "You can not consume that.");
		return;
	}

	if (!CONSUM(vial)->quantity) {
		notifyf(player, "%s is empty.", NAME(vial));
		return;
	}

	if (CONSUM(vial)->drink) {
		aux = ENTITY(player)->thirst - DRINK_VALUE(vial);
		ENTITY(player)->thirst = aux < 0 ? 0 : aux;
	}

	if (CONSUM(vial)->food) {
		aux = ENTITY(player)->hunger - FOOD_VALUE(vial);
		ENTITY(player)->hunger = aux < 0 ? 0 : aux;
	}

	CONSUM(vial)->quantity--;
	notify_wts(player, "consume", "consumes", " %s", NAME(vial));

	if (!CONSUM(vial)->quantity && !CONSUM(vial)->capacity)
		recycle(player, vial);
}

void
do_fill(command_t *cmd)
{
	dbref player = cmd->player;
	const char *vial_s = cmd->argv[1];
	const char *source_s = cmd->argv[2];
	dbref vial = ematch_mine(player, vial_s);
	unsigned m;

	if (vial == NOTHING) {
		notify(player, "Fill what?");
		return;
	}

	if (Typeof(vial) != TYPE_CONSUMABLE || !(m = CONSUM(vial)->capacity)) {
		notify(player, "You can not fill that up.");
		return;
	}

	if (CONSUM(vial)->quantity) {
		notifyf(player, "%s is not empty.", NAME(vial));
		return;
	}

	dbref source = ematch_near(player, source_s);

	if (source < 0) {
		notify(player, "Invalid source.");
		return;
	}

	CONSUM(vial)->quantity = m;
	CONSUM(vial)->drink = CONSUM(source)->drink;
	CONSUM(vial)->food = CONSUM(source)->food;

	notify_wts(player, "fill", "fills", " %s from %s",
		   NAME(vial), NAME(source));
}
