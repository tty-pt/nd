#ifndef KILL_H
#define KILL_H

#include "mob.h"
/* #include "db.h" */
/* #include "item.h" */
/* #include "params.h" */

void kill_update(struct mob *);
void kill_target(dbref attacker, dbref target);
int kill_dodge(struct mob *att, struct wts wts);
short kill_dmg(enum element dmg_type,
		short dmg, short def,
		enum element def_type);
void notify_attack(dbref att, dbref tar, struct wts wts,
		short val, char const *color, short mval);

#endif
