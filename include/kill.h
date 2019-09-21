#ifndef KILL_H
#define KILL_H

#include "db.h"
#include "item.h"
#include "params.h"

#define RARE_MAX 6

#define G(x) xsqrtx(x)
#define MSRA(ms, ra, G) G(ms) * (ra + 1) / RARE_MAX

#define IE(x, G) MSRA(GETMSV(x), GETRARE(x), G)
#define HS(sp) MSRA(sp->ms, sp->ra, SPELL_G)

#define DMG_G(v) G(v)
#define DODGE_G(v) G(v)

#define EV(liv, w) liv->e[AF_ ## w].value
#define EM(liv, w) liv->e[AF_ ## w].mask

struct living *living_get(dbref who);
struct living *living_put(dbref who);

void do_living_init(void);
void livings_update(void);
void do_living_save(void);

#endif
