#ifndef STAT_H
#define STAT_H

#include "entity.h"

#define RARE_MAX 6

#define MSRA(ms, ra, G) G(ms) * (ra + 1) / RARE_MAX

#define IE(equ, G) MSRA(equ->msv, equ->rare, G)
#define HS(sp) MSRA(sp->ms, sp->ra, SPELL_G)

#endif
