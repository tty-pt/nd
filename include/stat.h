#ifndef STAT_H
#define STAT_H

#include "math.h"

#define RARE_MAX 6

#define G(x) xsqrtx(x)
#define MSRA(ms, ra, G) G(ms) * (ra + 1) / RARE_MAX

#define IE(x, G) MSRA(GETMSV(x), GETRARE(x), G)
#define HS(sp) MSRA(sp->ms, sp->ra, SPELL_G)

#define DMG_G(v) G(v)
#define DODGE_G(v) G(v)

#define DMG_BASE(p) DMG_G(GETSTAT(p, STR))
#define DODGE_BASE(p) DODGE_G(GETSTAT(p, DEX))

#define HP_G(v) 10 * G(v)
#define HP_MAX(p) HP_G(GETSTAT(p, CON))

#define MP_G(v) HP_G(v)
#define MP_MAX(p) MP_G(GETSTAT(p, WIZ))

static inline unsigned
xsqrtx(unsigned x)
{
	// y * x ^ 3/2
	return x * sqrt(x);
}

#endif
