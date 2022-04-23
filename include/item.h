// TODO blocking, parry

#ifndef ITEM_H
#define ITEM_H

#include "geometry.h"
/* #include "mdb.h" */

#define ARMORSET_LIST(s) & s ## _helmet_drop, \
	& s ## _chest_drop, & s ## _pants_drop

struct wts {
	const char *a, *b;
};

extern struct wts phys_wts[];

int equip_affect(dbref who, dbref eq);
dbref unequip(dbref player, unsigned eql);
int equip(dbref player, dbref eq);

#endif
