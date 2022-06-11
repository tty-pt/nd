#ifndef KILL_H
#define KILL_H

#include "mob.h"
/* #include "db.h" */
/* #include "item.h" */
/* #include "params.h" */

void kill_update(OBJ *player);
OBJ *kill_target(OBJ *player, OBJ *target);
int kill_dodge(OBJ *player, struct wts wts);
short kill_dmg(enum element dmg_type,
		short dmg, short def,
		enum element def_type);
void notify_attack(OBJ *player, OBJ *target, struct wts wts,
		short val, char const *color, short mval);
void sit(OBJ *who, char const *what);
void stand(OBJ *who);

#endif
