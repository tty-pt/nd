#ifndef UAPI_OBJECT_H
#define UAPI_OBJECT_H

#include "./azoth.h"
#include <stdint.h>

#define ROOT ((unsigned) 1)
#define NOTHING ((unsigned) -1)

enum object_flags {
	OF_INF = 1,
};

enum room_flags {
	RF_TEMP = 1,
	RF_HAVEN = 2,
};

enum exit {
	E_NULL = 0,
	E_WEST = 1,
	E_NORTH = 2,
	E_UP = 4,
	E_EAST = 8,
	E_SOUTH = 16,
	E_DOWN = 32,
	E_ALL = 63,
};

typedef struct {
	unsigned flags;
	unsigned char exits;
	unsigned char doors;
	unsigned char floor;
} ROO;

enum entity_flags {
	EF_PLAYER = 1,
	EF_SHOP = 4,
	EF_WIZARD = 8,
	EF_BAN = 16,
};

enum attribute {
	ATTR_STR,
	ATTR_CON,
	ATTR_DEX,
	ATTR_INT,
	ATTR_WIZ,
	ATTR_CHA,
	ATTR_MAX
};

typedef struct entity {
	unsigned home;
	unsigned flags;

	/* tmp data? */
	unsigned last_observed;
	unsigned char select;
	unsigned char aux;
} ENT;

typedef struct object {
	unsigned location, owner;

	unsigned skid;
	unsigned art_id;
	unsigned char type;
	unsigned value;
	unsigned flags;
	char name[32];
	unsigned data[8];
} OBJ;

struct icon {
	int actions;
	struct print_info pi;
	char ch;
};

/* FIXME: not for plugins */
extern unsigned obj_hd, contents_hd, obs_hd;

typedef int obj_exists_t(unsigned ref);
obj_exists_t obj_exists;

typedef unsigned object_new_t(OBJ *obj);
object_new_t object_new;

typedef unsigned object_copy_t(OBJ *nu, unsigned old_ref);
object_copy_t object_copy;

typedef void object_move_t(unsigned what_ref, unsigned where_ref);
object_move_t object_move;

typedef unsigned object_add_t(OBJ *nu, unsigned skel_id, unsigned where, uint64_t v);
object_add_t object_add;

typedef void object_drop_t(unsigned where_ref, unsigned skel_id);
object_drop_t object_drop;

typedef struct icon object_icon_t(unsigned thing_ref);
object_icon_t object_icon;

typedef char *object_art_t(unsigned ref);
object_art_t object_art;

typedef const char *unparse_t(unsigned loc_ref);
unparse_t unparse;

#endif
