// TODO blocking, parry

#ifndef ITEM_H
#define ITEM_H

#include "geometry.h"
/* #include "mdb.h" */

#define ARMORSET_LIST(s) & s ## _helmet, \
	& s ## _chest, & s ## _pants

struct item {
	struct obj o;
	unsigned short eqw, msv;
};

struct wts {
	const char *a, *b;
};

extern struct wts phys_wts[];

/* dbref contents_find(int descr, dbref player, dbref what, const char *name); */
int equip_calc(dbref who, dbref eq);
dbref unequip(dbref player, unsigned eql);
int cannot_equip(dbref player, dbref eq);

#endif
