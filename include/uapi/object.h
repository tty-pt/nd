#ifndef UAPI_OBJECT_H
#define UAPI_OBJECT_H

#include <qhash.h>

#define ROOT ((unsigned) 1)
#define NOTHING ((unsigned) -1)

enum object_flags {
	OF_INF = 1,
};

enum type {
	TYPE_ROOM,
	TYPE_THING,
	TYPE_PLANT,
	TYPE_ENTITY,
	TYPE_EQUIPMENT,
	TYPE_CONSUMABLE,
	TYPE_SEAT,
	TYPE_MINERAL,
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
	EF_AGGRO = 2,
	EF_SITTING = 4,
	EF_SHOP = 8,
	EF_WIZARD = 16,
	EF_BUILDER = 32,
};

enum huth {
	HUTH_THIRST = 0,
	HUTH_HUNGER = 1,
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

enum equipment_slot {
	ES_HEAD,
	ES_NECK,
	ES_CHEST,
	ES_BACK,
	ES_RHAND,
	ES_LFINGER,
	ES_RFINGER,
	ES_PANTS,
	ES_MAX
};

struct debuf {
	int skel;
	unsigned duration;
	short val;
};

struct spell {
	unsigned skel;
	unsigned cost; 
	unsigned short val;
};

// insert spells skels here

enum spell_affects {
	// these are changed by bufs
	AF_HP,
	AF_MOV,
	AF_MDMG,
	AF_MDEF,
	AF_DODGE,

	// these aren't.
	AF_DMG,
	AF_DEF,

	// these are flags, not types of buf
	AF_NEG = 0x10,
	AF_BUF = 0x20,
};

struct effect {
	short value;
	unsigned char mask;
};

typedef struct entity {
	unsigned home;
        /* const char *dialog; <- make this external to the struct (use object id) */
	struct debuf debufs[8];
	struct spell spells[8];
	struct effect e[7];
	unsigned target, sat;
	unsigned flags;
	unsigned short hp, mp, wtso;
	unsigned short huth[2];
	unsigned char debuf_mask, combo, klock;
	unsigned lvl, spend, cxp;
	unsigned attr[ATTR_MAX];
	unsigned equipment[ES_MAX];

	/* tmp data? */
	unsigned last_observed;
	unsigned char select;
	unsigned char huth_n[2];
} ENT;

enum equipment_flags {
	EQF_EQUIPPED = 1,
};

enum armor_type {
	ARMOR_LIGHT,
	ARMOR_MEDIUM,
	ARMOR_HEAVY,
};

typedef struct {
	unsigned eqw;
	unsigned msv;
	unsigned rare;
	unsigned flags;
} EQU;

typedef struct {
	unsigned food;
	unsigned drink;
	unsigned quantity;
	unsigned capacity;
} CON;

typedef struct {
	unsigned plid;
	unsigned size;
} PLA;

typedef struct {
	unsigned quantity;
	unsigned capacity;
} SEA;

typedef struct {
	short unsigned unused;
} MIN;

union specific {
	ROO room;
	char entity;
	EQU equipment;
	CON consumable;
	PLA plant;
	SEA seat;
	MIN mineral;
};

typedef struct object {
	unsigned location, owner;

	unsigned art_id;
	unsigned char type;
	unsigned value;
	unsigned flags;

	union specific sp;

	char *name, *description;
	// int skel;
} OBJ;

struct icon {
	int actions;
	char *icon;
};

extern unsigned obj_hd, contents_hd, obs_hd;

typedef int obj_exists_t(unsigned ref);
obj_exists_t obj_exists;

typedef unsigned object_new_t(OBJ *obj);
object_new_t object_new;

typedef unsigned object_copy_t(OBJ *nu, unsigned old_ref);
object_copy_t object_copy;

typedef void object_move_t(unsigned what_ref, unsigned where_ref);
object_move_t object_move;

typedef unsigned object_add_t(OBJ *nu, unsigned skel_id, unsigned where, void *arg);
object_add_t object_add;

typedef void object_drop_t(unsigned where_ref, unsigned skel_id);
object_drop_t object_drop;

static inline int
object_item(unsigned obj_ref)
{
	OBJ obj;
	uhash_get(obj_hd, &obj, obj_ref);
	return obj.type == TYPE_THING
		|| obj.type == TYPE_CONSUMABLE
		|| obj.type == TYPE_EQUIPMENT;
}

typedef struct icon object_icon_t(unsigned thing_ref);
object_icon_t object_icon;

typedef unsigned art_max_t(char *name);
art_max_t art_max;

typedef char *object_art_t(unsigned ref);
object_art_t object_art;

typedef const char *unparse_t(unsigned loc_ref);
unparse_t unparse;

#endif
