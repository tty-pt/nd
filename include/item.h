// TODO blocking, parry

#ifndef ITEM_H
#define ITEM_H

#include "geometry.h"
/* #include "mdb.h" */

#define ARMORSET_LIST(s) & s ## _helmet_drop, \
	& s ## _chest_drop, & s ## _pants_drop

extern struct wts phys_wts[];

int equip_affect(OBJ *who, OBJ *eq);
dbref unequip(OBJ *player, unsigned eql);
int equip(OBJ *player, OBJ *eq);

#endif
