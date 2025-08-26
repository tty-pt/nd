#ifndef UAPI_ENTITY_H
#define UAPI_ENTITY_H

#include <math.h>
#include "./skel.h"
#include "./object.h"

typedef unsigned me_get_t(void);
me_get_t me_get;

typedef ENT ent_get_t(unsigned ref);
ent_get_t ent_get;

typedef void ent_set_t(unsigned ref, ENT *tmp);
ent_set_t ent_set;

typedef void ent_del_t(unsigned ref);
ent_del_t ent_del;

typedef int controls_t(unsigned who_ref, unsigned what_ref);
controls_t controls;

typedef int payfor_t(unsigned who_ref, OBJ *who, unsigned cost);
payfor_t payfor;

typedef void enter_t(unsigned player_ref, unsigned loc_ref, enum exit e);
enter_t enter;

typedef void look_at_t(unsigned player_ref, unsigned loc_ref);
look_at_t look_at;

extern unsigned me;
extern unsigned ent_hd;

#endif
