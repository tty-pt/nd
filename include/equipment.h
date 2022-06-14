#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "attr.h"

#define DMG_WEAPON(equ) IE(equ, DMG_G)
#define WTS_WEAPON(equ) phys_wts[EQT(equ->eqw)]

#define DEF_G(v) G(v)
#define DEF_ARMOR(equ, aux) (IE(equ, DEF_G) >> aux)
#define DODGE_ARMOR(def) def / 4

#endif
