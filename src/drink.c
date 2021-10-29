#include "drink.h"

#include <stddef.h>
#include "mob.h"

#include "externs.h"

drink_t drink_map[] = {
	[DRINK_WATER] = {
		"water", DA_NONE,
	},
	[DRINK_MILK] = {
		"milk", DA_HP,
	},
	[DRINK_AYUHASCA] = {
		"ayuhasca", DA_MP,
	},
};

static inline int
drink(dbref who, dbref source)
{
	static const size_t affects_ofs[] = {
		0,
		offsetof(struct mob, hp),
		offsetof(struct mob, mp),
	};

	struct mob *mob = MOB(who);
	drink_t *drink = DRINK_SOURCE(source);
	unsigned n = GETCONSUN(source);
	unsigned const value = 2;
	int aux;

	if (!n) {
		notifyf(who, "%s is empty.", NAME(source));
		return 1;
	}

	* (unsigned *) (((char *) mob) + affects_ofs[drink->da]) += 1 << value;
	aux = mob->thirst - (1 << (16 - value));
	if (aux < 0)
		aux = 0;
	mob->thirst = aux;

	SETCONSUN(source, n - 1);
	notify_wts(who, "drink", "drinks", " %s", drink->name);
	return 0;
}

void
do_drink(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref vial;


	if (!*name) {
		notify(player, "Drink what?");
		return;
	}

	vial = contents_find(cmd, player, name);

	if (vial == NOTHING) {
		notify(player, "Drink what?");
		return;
	}

	drink(player, vial);
	/* if (drink(player, vial)) { */
	/* 	notify(player, "You can't drink from that."); */
	/* 	return; */
	/* } */
}

void
do_fill(command_t *cmd)
{
	dbref player = cmd->player;
	const char *vial_s = cmd->argv[1];
	const char *source_s = cmd->argv[2];
	dbref vial = contents_find(cmd, player, vial_s);
	unsigned m, n;

	if (vial == NOTHING) {
		notify(player, "Fill what?");
		return;
	}

	m = GETCONSUM(vial);
	if (!m) {
		notifyf(player, "You can't fill %s.", NAME(vial));
		return;
	}

	n = GETCONSUN(vial);
	if (n) {
		notifyf(player, "%s is not empty.", NAME(vial));
		return;
	}

	dbref source = contents_find(cmd, getloc(player), source_s);

	if (source < 0) {
		notify(player, "Invalid source.");
		return;
	}

	int dr = GETDRINK(source);

	if (dr < 0) {
		notify(player, "Invalid source.");
		return;
	}

	SETCONSUN(vial, m);
	SETDRINK(vial, dr);

	notify_wts(player, "fill", "fills", " %s with %s from %s",
		   NAME(vial), DRINK(dr)->name, NAME(source));
}

