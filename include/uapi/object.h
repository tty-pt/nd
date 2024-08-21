#ifndef UAPI_OBJECT_H
#define UAPI_OBJECT_H

typedef int dbref;

enum type {
	TYPE_ROOM,
	TYPE_THING,
	TYPE_PLANT,
	TYPE_ENTITY,
	TYPE_EQUIPMENT,
	TYPE_CONSUMABLE,
	TYPE_GARBAGE,
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
	int skel;
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
	dbref home;
	dbref last_observed;
        /* const char *dialog; <- make this external to the struct (use object id) */
	struct debuf debufs[8];
	struct spell spells[8];
	struct effect e[7];
	dbref target, sat;
	unsigned flags;
	unsigned short hp, mp, hunger, thirst, wtso;
	unsigned char debuf_mask, combo, select, klock;
	unsigned lvl, spend, cxp;
	unsigned attr[ATTR_MAX];
	unsigned equipment[ES_MAX];
	unsigned char hunger_n, thirst_n;
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
	ENT entity;
	EQU equipment;
	CON consumable;
	PLA plant;
	SEA seat;
	MIN mineral;
};

typedef struct object {
	long observers;
	dbref location, owner;

	unsigned art_id;
	unsigned char type;
	unsigned value;
	unsigned flags;

	union specific sp;

	char *name, *description;
	// int skel;
} OBJ;

#endif
