#ifndef STAT_H
#define STAT_H

#include "math.h"

#define RARE_MAX 6

#define G(x) xsqrtx(x)
#define MSRA(ms, ra, G) G(ms) * (ra + 1) / RARE_MAX

#define IE(equ, G) MSRA(equ->msv, equ->rare, G)
#define HS(sp) MSRA(sp->ms, sp->ra, SPELL_G)

#define DMG_G(v) G(v)
#define DODGE_G(v) G(v)

#define DMG_BASE(p) DMG_G(ATTR(p, ATTR_STR))
#define DODGE_BASE(p) DODGE_G(ATTR(p, ATTR_DEX))

#define HP_G(v) 10 * G(v)
#define HP_MAX(ent) HP_G(ent->attr[ATTR_CON])

#define MP_G(v) HP_G(v)
#define MP_MAX(ent) MP_G(ent->attr[ATTR_WIZ])

static inline unsigned
xsqrtx(unsigned x)
{
	// y * x ^ 3/2
	return x * sqrt(x);
}

#endif
